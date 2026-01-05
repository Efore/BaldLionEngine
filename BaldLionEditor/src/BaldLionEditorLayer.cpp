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
			m_baldLionGameLayer = MemoryManager::New<BaldLion::Game::BaldLionGameLayer>("BaldLionGameLayer", AllocationType::FreeList_Main);
		}

		BaldLionEditorLayer::~BaldLionEditorLayer()
		{
			if (m_viewportCameraTransform)
			{
				BaldLion::Memory::MemoryManager::Delete(m_viewportCameraTransform);
			}

			if (m_viewportCamera)
			{
				BaldLion::Memory::MemoryManager::Delete(m_viewportCamera);
			}
		}

		void BaldLionEditorLayer::OnActivate()
		{
			BL_PROFILE_FUNCTION();

			m_isActive = true;
			Physics::PhysicsManager::SetIsPhysicsActive(false);

			EventManager::RegisterEventHandler("WindowResizedEvent", BL_BIND_OBJECT_FUNCTION(BaldLionEditorLayer::OnWindowResizedEvent));
			EventManager::RegisterEventHandler("KeyPressedEvent", BL_BIND_OBJECT_FUNCTION(BaldLionEditorLayer::OnKeyPressedEvent));

			if (SceneManagement::SceneManager::GetMainScenePathFile().empty() || 
				!SceneManagement::SceneManager::OpenScene(SceneManagement::SceneManager::GetMainScenePathFile().c_str()))
			{
				NewScene();
			}

			m_ecsManager = SceneManagement::SceneManager::GetECSManager();

			//Panel Setup
			m_entityPropertiesPanel.SetHierarchyPanel(&m_sceneHierarchyPanel);
			m_editorViewportPanel.SetHierarchyPanel(&m_sceneHierarchyPanel);
			m_navigationPanel.SetViewportPanel(&m_editorViewportPanel);

			ECS::SingletonSystems::CameraSystem::SetMainCamera(m_viewportCamera, m_viewportCameraTransform);
		}

		void BaldLionEditorLayer::OnDeactivate()
		{		
			m_isActive = false;
			EventManager::UnregisterEventHandler("WindowResizedEvent", BL_BIND_OBJECT_FUNCTION(BaldLionEditorLayer::OnWindowResizedEvent));
			EventManager::UnregisterEventHandler("KeyPressedEvent", BL_BIND_OBJECT_FUNCTION(BaldLionEditorLayer::OnKeyPressedEvent));
		}

		void BaldLionEditorLayer::OnUpdate()
		{
			BL_PROFILE_FUNCTION();


			HandleInput();

			SceneManagement::SceneManager::FrameStart();
			SceneManagement::SceneManager::Update(0.0f);
			SceneManagement::SceneManager::FrameEnd();

			Renderer::BeginScene();			
			Renderer::DrawScene();
			Renderer::EndScene();	

		}

		void BaldLionEditorLayer::OnImGuiRender()
		{
			BL_PROFILE_FUNCTION();

			RenderDockSpace();			

			const float deltaTime = (float)m_gameStateTimer.GetDeltaTime();

			m_sceneHierarchyPanel.OnImGuiRender(deltaTime);
			m_editorViewportPanel.OnImGuiRender(deltaTime);
			m_entityPropertiesPanel.OnImGuiRender(deltaTime);
			m_memoryAllocationPanel.OnImGuiRender(deltaTime);
			m_inputManagementPanel.OnImGuiRender(deltaTime);
			m_renderingDataPanel.OnImGuiRender(deltaTime);
			m_resourcesPanel.OnImGuiRender(deltaTime);
			m_animatorPanel.OnImGuiRender(deltaTime);
			m_navigationPanel.OnImGuiRender(deltaTime);
			m_htnPanel.OnImGuiRender(deltaTime);
		}

		void BaldLionEditorLayer::SetupEditorCamera()
		{
			m_viewportCameraTransform = BaldLion::Memory::MemoryManager::New<ECS::ECSTransformComponent>("Editor camera transform", AllocationType::FreeList_ECS,
				MathUtils::Vector3Zero,
				MathUtils::Vector3Zero,
				glm::vec3(1.0f));

			m_viewportCamera = BaldLion::Memory::MemoryManager::New<ECS::ECSProjectionCameraComponent>("Editor camera", AllocationType::FreeList_ECS,
				45.0f,
				(float)Application::GetInstance().GetWindow().GetWidth(),
				(float)Application::GetInstance().GetWindow().GetHeight(),
				0.1f,
				1000.0f);

			m_cameraMovementSpeed = 25.0f;
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

				if (ImGui::BeginMenu("Game"))
				{
					if (m_isActive)
					{
						if (ImGui::MenuItem("Start Game"))
						{
							StartGame();
						}
					}
					else 
					{
						if (ImGui::MenuItem("Stop Game"))
						{
							StopGame();
						}
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			ImGui::End();
		}

		bool BaldLionEditorLayer::OnWindowResizedEvent(const EventEntry& e)
		{
			ui32 width = e.eventData1;
			ui32 height = e.eventData2;

			ECS::SingletonSystems::CameraSystem::SetCameraSize((float)width, (float)height);

			Renderer::OnWindowResized(width, height);

			return true;
		}
		
		bool BaldLionEditorLayer::OnKeyPressedEvent(const EventEntry& e)
		{
			if ((ui32)e.eventData2 > 0)
				return false;

			switch ((ui32)e.eventData1)
			{
			case BL_KEY_S:
				if (Input::PlatformInput::IsKeyPressed(BL_KEY_LEFT_CONTROL) || Input::PlatformInput::IsKeyPressed(BL_KEY_RIGHT_CONTROL))
				{
					if (Input::PlatformInput::IsKeyPressed(BL_KEY_LEFT_SHIFT) || Input::PlatformInput::IsKeyPressed(BL_KEY_RIGHT_SHIFT)) 
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
				if (Input::PlatformInput::IsKeyPressed(BL_KEY_LEFT_CONTROL) || Input::PlatformInput::IsKeyPressed(BL_KEY_RIGHT_CONTROL))
				{
					NewScene();
					return true;
				}
				break;
			case BL_KEY_O:
				if (Input::PlatformInput::IsKeyPressed(BL_KEY_LEFT_CONTROL) || Input::PlatformInput::IsKeyPressed(BL_KEY_RIGHT_CONTROL))
				{
					OpenScene();
					return true;
				}
				break;
			default:
				break;
			}

			m_sceneHierarchyPanel.OnKeyPressed((ui32)e.eventData1);
			m_editorViewportPanel.OnKeyPressed((ui32)e.eventData1);
			m_entityPropertiesPanel.OnKeyPressed((ui32)e.eventData1);
			m_memoryAllocationPanel.OnKeyPressed((ui32)e.eventData1);
			m_renderingDataPanel.OnKeyPressed((ui32)e.eventData1);

			return false;
		}

		void BaldLionEditorLayer::OpenScene()
		{
			std::string filepath = FileDialogs::OpenFile("BaldLion Scene (*.scene)\0*.scene\0");

			if (!filepath.empty())
			{
				SceneManagement::SceneManager::OpenScene(filepath.c_str());
				m_ecsManager = SceneManagement::SceneManager::GetECSManager();			
			}
		}

		void BaldLionEditorLayer::SaveScene()
		{
			if (!SceneManagement::SceneManager::GetMainScenePathFile().empty())
			{
				SceneManagement::SceneManager::SaveScene(SceneManagement::SceneManager::GetMainScenePathFile().c_str());
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
			}
		}

		void BaldLionEditorLayer::NewScene()
		{
			SceneManagement::SceneManager::SetMainScenePathFile("");
			SceneManagement::SceneManager::AddScene("UnNamed Scene");
			m_ecsManager = SceneManagement::SceneManager::GetECSManager();

			{//Directional Light setup

				ECS::ECSEntityID directionalLight = m_ecsManager->AddEntity("Directional Light");
				ECS::ECSDirectionalLightComponent* directionalLightComponent = m_ecsManager->CreateComponent<ECS::ECSDirectionalLightComponent>(
					ECS::ECSComponentType::DirectionalLight,
					glm::vec3(0.0f),
					glm::vec3(1.0f),
					glm::vec3(0.2f));

				ECS::ECSTransformComponent* directionalLightTransformComponent = m_ecsManager->CreateComponent<ECS::ECSTransformComponent>(
					ECS::ECSComponentType::Transform,
					glm::vec3(0.0f),
					glm::vec3(0.0f),
					glm::vec3(1.0f));

				m_ecsManager->AddComponentToEntity(directionalLight, directionalLightComponent);
				m_ecsManager->AddComponentToEntity(directionalLight, directionalLightTransformComponent);

				ECS::SingletonSystems::LightningSystem::SetDirectionalLight(directionalLightComponent);
				ECS::SingletonSystems::LightningSystem::SetDirectionalLightTransform(directionalLightTransformComponent);
			}
		}

		void BaldLionEditorLayer::StartGame()
		{
			SaveScene();
			Application::GetInstance("").PushLayer(m_baldLionGameLayer);
		}

		void BaldLionEditorLayer::StopGame()
		{
			Application::GetInstance("").PopLayer(m_baldLionGameLayer);
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

			if (BaldLion::Input::PlatformInput::IsMouseButtonPress(BL_MOUSE_BUTTON_2))
			{				
				if (BaldLion::Input::PlatformInput::IsKeyPressed(BL_KEY_W))
					cameraMovement -= MathUtils::GetTransformForwardDirection(cameraTransform) * (float)m_gameStateTimer.GetDeltaTime() * cameraMovementSpeed;
				else if (BaldLion::Input::PlatformInput::IsKeyPressed(BL_KEY_S))
					cameraMovement += MathUtils::GetTransformForwardDirection(cameraTransform) * (float)m_gameStateTimer.GetDeltaTime() * cameraMovementSpeed;

				if (BaldLion::Input::PlatformInput::IsKeyPressed(BL_KEY_A))
					cameraMovement -= MathUtils::GetTransformRightDirection(cameraTransform) * (float)m_gameStateTimer.GetDeltaTime() * cameraMovementSpeed;
				else if (BaldLion::Input::PlatformInput::IsKeyPressed(BL_KEY_D))
					cameraMovement += MathUtils::GetTransformRightDirection(cameraTransform) * (float)m_gameStateTimer.GetDeltaTime() * cameraMovementSpeed;

				if (BaldLion::Input::PlatformInput::IsKeyPressed(BL_KEY_Q))
					cameraMovement -= MathUtils::Vector3UnitY * (float)m_gameStateTimer.GetDeltaTime() * cameraMovementSpeed;
				else if (BaldLion::Input::PlatformInput::IsKeyPressed(BL_KEY_E))
					cameraMovement += MathUtils::Vector3UnitY * (float)m_gameStateTimer.GetDeltaTime() * cameraMovementSpeed;

				if (BaldLion::Input::PlatformInput::IsKeyPressed(BL_KEY_LEFT_SHIFT))
					cameraMovement *= 2;
			}
		}

		void BaldLionEditorLayer::CalculateCameraRotation(const float cameraRotationSpeed, float& prevX, float& prevY, float& cameraYaw, float& cameraPitch)
		{
			if (BaldLion::Input::PlatformInput::IsMouseButtonPress(BL_MOUSE_BUTTON_2))
			{
				float deltaX = BaldLion::Input::PlatformInput::GetMouseX() - prevX;
				float deltaY = BaldLion::Input::PlatformInput::GetMouseY() - prevY;

				cameraYaw -= deltaX * cameraRotationSpeed * (float)m_gameStateTimer.GetDeltaTime();
				cameraPitch -= deltaY * cameraRotationSpeed * (float)m_gameStateTimer.GetDeltaTime();
			}

			prevX = BaldLion::Input::PlatformInput::GetMouseX();
			prevY = BaldLion::Input::PlatformInput::GetMouseY();
		}
	}
}