#include "BaldLionEditorLayer.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include "ImGui/imgui.h"
#include "ImGuizmo.h"
#include "BaldLion/Utils/PlatformUtils.h"
#include "BaldLion/SceneManagement/Serialization/SceneSerializer.h"

namespace BaldLion
{
	namespace Editor
	{	
		BaldLionEditorLayer::BaldLionEditorLayer()			
		{
			SetupEditorCamera();

			BL_PROFILE_FUNCTION();
			Physics::PhysicsManager::SetIsPhysicsActive(false);
			SceneManagement::SceneManager::AddScene("UnNamed Scene");
			m_currentScenePathFile = "";
			m_ecsManager = SceneManagement::SceneManager::GetECSManager();

			{//Directional Light setup

				ECS::ECSEntityID directionalLight = m_ecsManager->AddEntity("Directional Light");
				ECS::ECSDirectionalLightComponent* directionalLightComponent = m_ecsManager->CreateComponent<ECS::ECSDirectionalLightComponent>(
					ECS::ECSComponentType::DirectionalLight,
					glm::vec3(0.0f),
					glm::vec3(1.0f),
					glm::vec3(0.2f),
					glm::vec3(-1.0f, -1.0f, 1.0f));

				m_ecsManager->AddComponentToEntity(directionalLight, directionalLightComponent);

				ECS::SingletonComponents::ECSLightSingleton::SetDirectionalLight(directionalLightComponent);
			}

			//Panel Setup
			m_sceneHierarchyPanel.SetSceneContext(SceneManagement::SceneManager::GetMainScene());
			m_entityPropertiesPanel.SetHierarchyPanel(&m_sceneHierarchyPanel);
			m_editorViewportPanel.SetHierarchyPanel(&m_sceneHierarchyPanel);			
			m_navigationPanel.SetNavigationPanel(&m_editorViewportPanel);
		}

		BaldLionEditorLayer::~BaldLionEditorLayer()
		{
			SceneManagement::SceneManager::Stop();

			if (m_viewportCameraTransform)
			{
				BaldLion::Memory::MemoryManager::Delete(m_viewportCameraTransform);
			}

			if (m_viewportCamera)
			{
				BaldLion::Memory::MemoryManager::Delete(m_viewportCamera);
			}
		}

		void BaldLionEditorLayer::OnAttach()
		{
			ECS::SingletonComponents::ECSProjectionCameraSingleton::Init();
			ECS::SingletonComponents::ECSProjectionCameraSingleton::SetMainCamera(m_viewportCamera, m_viewportCameraTransform);
		}

		void BaldLionEditorLayer::OnDetach()
		{		
		}

		void BaldLionEditorLayer::OnUpdate()
		{
			BL_PROFILE_FUNCTION();

			SceneManagement::SceneManager::FrameStart();
			SceneManagement::SceneManager::Update();
				
			Renderer::BeginScene();			
			Renderer::DrawScene();
			Renderer::EndScene();		

			SceneManagement::SceneManager::FrameEnd();
		}

		void BaldLionEditorLayer::OnImGuiRender()
		{
			BL_PROFILE_FUNCTION();

			RenderDockSpace();			

			m_sceneHierarchyPanel.OnImGuiRender();
			m_editorViewportPanel.OnImGuiRender();
			m_entityPropertiesPanel.OnImGuiRender(); 
			m_memoryAllocationPanel.OnImGuiRender();
			m_renderingDataPanel.OnImGuiRender();
			m_resourcesPanel.OnImGuiRender();
			m_animatorPanel.OnImGuiRender();
			m_navigationPanel.OnImGuiRender();

			HandleInput();
		}

		void BaldLionEditorLayer::OnEvent(Event& e)
		{
			BaldLion::EventDispatcher dispatcher(e);
			
			dispatcher.Dispatch<BaldLion::WindowResizeEvent>(BL_BIND_FUNCTION(BaldLionEditorLayer::OnWindowResizeEvent));
			dispatcher.Dispatch<BaldLion::KeyPressedEvent>(BL_BIND_FUNCTION(BaldLionEditorLayer::OnKeyPressedEvent));
		}

		void BaldLionEditorLayer::SetupEditorCamera()
		{
			m_viewportCameraTransform = BaldLion::Memory::MemoryManager::New<ECS::ECSTransformComponent>("Editor camera transform", AllocationType::FreeList_ECS,
				glm::vec3(0, 10, -50),
				MathUtils::Vector3Zero,
				glm::vec3(1.0f));

			m_viewportCamera = BaldLion::Memory::MemoryManager::New<ECS::ECSProjectionCameraComponent>("Editor camera", AllocationType::FreeList_ECS,
				45.0f,
				(float)Application::GetInstance().GetWindow().GetWidth(),
				(float)Application::GetInstance().GetWindow().GetHeight(),
				0.1f,
				50000.0f);

			m_cameraMovementSpeed = 50.0f;
			m_cameraRotationSpeed = 1.0f;

			m_prevX = 0.0f;
			m_prevY = 0.0f;
			m_cameraYaw = 180.0f;
			m_cameraPitch = 0.0f;
		}

		void BaldLionEditorLayer::RenderDockSpace()
		{
			static bool dockSpaceOpen = true;
			static bool opt_fullscreen_persistant = true;
			static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
			bool opt_fullscreen = opt_fullscreen_persistant;

			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
			// because it would be confusing to have two docking targets within each others.
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			if (opt_fullscreen)
			{
				ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->Pos);
				ImGui::SetNextWindowSize(viewport->Size);
				ImGui::SetNextWindowViewport(viewport->ID);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			}

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace Demo", &dockSpaceOpen, window_flags);
			ImGui::PopStyleVar();

			if (opt_fullscreen)
				ImGui::PopStyleVar(2);

			// Dockspace
			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
			}
			else
			{
				//ShowDockingDisabledMessage();
			}

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New scene", "Ctrl+N"))
					{
						NewScene();
					}

					if (ImGui::MenuItem("Open scene", "Ctrl+O"))
					{
						OpenScene();
					}

					if (ImGui::MenuItem("Save scene","Ctrl+Shift+S"))
					{
						SaveScene();
					}

					if (ImGui::MenuItem("Save scene as..", "Ctrl+S"))
					{
						SaveSceneAs();
					}

					if (ImGui::MenuItem("Exit"))
					{
						BaldLion::Application::GetInstance().Close();
					}

					ImGui::EndMenu();
				}


				if (ImGui::BeginMenu("Physics"))
				{
					if (ImGui::MenuItem("Activate Phyisics"))
					{
						Physics::PhysicsManager::SetIsPhysicsActive(true);
					}

					if (ImGui::MenuItem("Deactivate Phyisics"))
					{
						Physics::PhysicsManager::SetIsPhysicsActive(false);
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			ImGui::End();
		}

		bool BaldLionEditorLayer::OnWindowResizeEvent(WindowResizeEvent& e)
		{
			ui32 width = e.GetWidth();
			ui32 height = e.GetHeight();

			ECS::SingletonComponents::ECSProjectionCameraSingleton::SetCameraSize((float)width, (float)height);

			Renderer::OnWindowResize(width, height);

			return true;
		}
		
		bool BaldLionEditorLayer::OnKeyPressedEvent(KeyPressedEvent& e)
		{
			if (e.GetRepeatCount() > 0)
				return false;

			switch (e.GetKeyCode())
			{
			case BL_KEY_S:
				if (Input::IsKeyPressed(BL_KEY_LEFT_CONTROL) || Input::IsKeyPressed(BL_KEY_RIGHT_CONTROL))
				{
					if (Input::IsKeyPressed(BL_KEY_LEFT_SHIFT) || Input::IsKeyPressed(BL_KEY_RIGHT_SHIFT)) 
					{
						SaveSceneAs();
						return true;
					}
					else 
					{
						SaveScene();
						return true;
					}
				}
				break;
			case BL_KEY_N:
				if (Input::IsKeyPressed(BL_KEY_LEFT_CONTROL) || Input::IsKeyPressed(BL_KEY_RIGHT_CONTROL))
				{
					NewScene();
					return true;
				}
				break;
			case BL_KEY_O:
				if (Input::IsKeyPressed(BL_KEY_LEFT_CONTROL) || Input::IsKeyPressed(BL_KEY_RIGHT_CONTROL))
				{
					OpenScene();
					return true;
				}
				break;
			default:
				break;
			}


			m_sceneHierarchyPanel.OnKeyPressed(e.GetKeyCode());
			m_editorViewportPanel.OnKeyPressed(e.GetKeyCode());
			m_entityPropertiesPanel.OnKeyPressed(e.GetKeyCode());
			m_memoryAllocationPanel.OnKeyPressed(e.GetKeyCode());
			m_renderingDataPanel.OnKeyPressed(e.GetKeyCode());

			return false;
		}

		void BaldLionEditorLayer::OpenScene()
		{
			std::string filepath = FileDialogs::OpenFile("BaldLion Scene (*.scene)\0*.scene\0");

			if (!filepath.empty())
			{
				SceneManagement::SceneManager::OpenScene(filepath.c_str());
				m_ecsManager = SceneManagement::SceneManager::GetECSManager();
				m_sceneHierarchyPanel.SetSceneContext(SceneManagement::SceneManager::GetMainScene());
				m_currentScenePathFile = filepath;
			}
		}

		void BaldLionEditorLayer::SaveScene()
		{
			if (!m_currentScenePathFile.empty())
			{
				SceneManagement::SceneManager::SaveScene(m_currentScenePathFile.c_str());
			}
			else
			{
				SaveSceneAs();
			}
		}

		void BaldLionEditorLayer::SaveSceneAs()
		{
			std::string filepath = FileDialogs::SaveFile("BaldLion Scene (*.scene)\0*.scene\0");
			if (!filepath.empty())
			{
				SceneManagement::SceneManager::GetMainScene()->SetSceneName(StringUtils::GetFileNameFromPath(filepath).c_str());
				SceneManagement::SceneManager::SaveScene(filepath.c_str());
				m_currentScenePathFile = filepath;
			}
		}

		void BaldLionEditorLayer::NewScene()
		{
			m_currentScenePathFile = "";

			SceneManagement::SceneManager::AddScene("UnNamed Scene");
			m_ecsManager = SceneManagement::SceneManager::GetECSManager();
			m_sceneHierarchyPanel.SetSceneContext(SceneManagement::SceneManager::GetMainScene());
		}


		void BaldLionEditorLayer::HandleInput()
		{
			MoveViewportCamera();
		}

		void BaldLionEditorLayer::MoveViewportCamera()
		{
			BL_PROFILE_FUNCTION();

			glm::mat4 cameraMatrixTransform = m_viewportCameraTransform->GetTransformMatrix();

			glm::vec3 cameraMovement;

			CalculateCameraMovement(m_cameraMovementSpeed, cameraMatrixTransform, cameraMovement);
			CalculateCameraRotation(m_cameraMovementSpeed, m_prevX, m_prevY, m_cameraYaw, m_cameraPitch);

			m_viewportCameraTransform->position += cameraMovement;
			m_viewportCameraTransform->rotation = glm::vec3(glm::radians(m_cameraPitch), glm::radians(m_cameraYaw), 0.0f);

			cameraMatrixTransform = m_viewportCameraTransform->GetTransformMatrix();

			const glm::mat4 viewMatrix = glm::inverse(cameraMatrixTransform);
			const glm::mat4 projectionMatrix = glm::perspective(glm::radians(m_viewportCamera->fov), m_viewportCamera->width / m_viewportCamera->height, m_viewportCamera->nearPlane, m_viewportCamera->farPlane);

			m_viewportCamera->viewProjectionMatrix = projectionMatrix * viewMatrix;
		}

		void BaldLionEditorLayer::CalculateCameraMovement(const float cameraMovementSpeed, const glm::mat4& cameraTransform, glm::vec3& cameraMovement)
		{
			cameraMovement = glm::vec3(0, 0, 0);

			if (BaldLion::Input::IsMouseButtonPress(BL_MOUSE_BUTTON_2))
			{
				if (BaldLion::Input::IsKeyPressed(BL_KEY_W))
					cameraMovement -= MathUtils::GetTransformForwardDirection(cameraTransform) * Time::GetDeltaTime() * cameraMovementSpeed;
				else if (BaldLion::Input::IsKeyPressed(BL_KEY_S))
					cameraMovement += MathUtils::GetTransformForwardDirection(cameraTransform) * Time::GetDeltaTime() * cameraMovementSpeed;

				if (BaldLion::Input::IsKeyPressed(BL_KEY_A))
					cameraMovement -= MathUtils::GetTransformRightDirection(cameraTransform) * Time::GetDeltaTime() * cameraMovementSpeed;
				else if (BaldLion::Input::IsKeyPressed(BL_KEY_D))
					cameraMovement += MathUtils::GetTransformRightDirection(cameraTransform) * Time::GetDeltaTime() * cameraMovementSpeed;

				if (BaldLion::Input::IsKeyPressed(BL_KEY_LEFT_SHIFT))
					cameraMovement *= 2;
			}
		}

		void BaldLionEditorLayer::CalculateCameraRotation(const float cameraRotationSpeed, float& prevX, float& prevY, float& cameraYaw, float& cameraPitch)
		{
			if (BaldLion::Input::IsMouseButtonPress(BL_MOUSE_BUTTON_2))
			{
				float deltaX = BaldLion::Input::GetMouseX() - prevX;
				float deltaY = BaldLion::Input::GetMouseY() - prevY;

				cameraYaw -= deltaX * cameraRotationSpeed * Time::GetDeltaTime();
				cameraPitch -= deltaY * cameraRotationSpeed * Time::GetDeltaTime();
			}

			prevX = BaldLion::Input::GetMouseX();
			prevY = BaldLion::Input::GetMouseY();
		}
	}
}