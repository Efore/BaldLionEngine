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
			
		}

		void BaldLionEditorLayer::OnAttach()
		{
			BL_PROFILE_FUNCTION();		

			SceneManagement::SceneManager::AddScene("UnNamed Scene");
			m_currentScenePathFile = "";
			m_ecsManager = SceneManagement::SceneManager::GetECSManager();

			//{//Camera setup
			//	
			//	ECS::ECSEntityID cameraEntity = m_ecsManager->AddEntity("Main Camera");

			//	ECS::ECSTransformComponent* cameraTransformComponent = m_ecsManager->AddComponent<ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform,
			//		glm::vec3(0, 5, 180),
			//		MathUtils::Vector3Zero,
			//		glm::vec3(1.0f));

			//	ECS::ECSProjectionCameraComponent* projectionCameraComponent = m_ecsManager->AddComponent<ECS::ECSProjectionCameraComponent>(ECS::ECSComponentType::ProjectionCamera,
			//		45.0f,
			//		(float)Application::GetInstance().GetWindow().GetWidth(),
			//		(float)Application::GetInstance().GetWindow().GetHeight(),
			//		0.1f,
			//		50000.0f);

			//	m_ecsManager->AddComponentToEntity(cameraEntity, cameraTransformComponent);
			//	m_ecsManager->AddComponentToEntity(cameraEntity, projectionCameraComponent);
			//}

			{//Directional Light setup

				ECS::ECSEntityID directionalLight = m_ecsManager->AddEntity("Directional Light");
				ECS::ECSDirectionalLightComponent* directionalLightComponent = m_ecsManager->AddComponent<ECS::ECSDirectionalLightComponent>(
					ECS::ECSComponentType::DirectionalLight,
					glm::vec3(0.0f),
					glm::vec3(1.0f),
					glm::vec3(0.2f),
					glm::vec3(-1.0f, -1.0f, -1.0f));

				m_ecsManager->AddComponentToEntity(directionalLight, directionalLightComponent);

				ECS::SingletonComponents::ECSLightSingleton::SetDirectionalLight(directionalLightComponent);
			}
			
			//{//Plane setup
			//	ECS::ECSEntityID planeEntity = m_ecsManager->AddEntity("Plane");
			//	ECS::ECSTransformComponent* planeTransformComponent = m_ecsManager->AddComponent<ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform,
			//		glm::vec3(0, 0, 0),
			//		MathUtils::Vector3Zero,
			//		glm::vec3(10.0f, 1.0f, 10.0f));

			//	Texture* gridTexture =  ResourceManagement::ResourceManager::AddResource<Texture>("assets/textures/TextureGrid.png", ResourceManagement::ResourceType::Texture);				

			//	gridTexture->SetWrapMode(WrapMode::Repeat, WrapMode::Repeat);

			//	Rendering::Material::MaterialProperties shapeMaterialProperties{
			//			STRING_TO_STRINGID("assets/shaders/baseLit.glsl"),
			//			glm::vec3(1.0f),
			//			glm::vec3(1.0f),
			//			glm::vec3(1.0f),
			//			glm::vec3(1.0f),
			//			32.0f,
			//			nullptr,
			//			gridTexture,
			//			nullptr,
			//			nullptr,
			//			nullptr,
			//			Material::BlendMode::None,
			//			Material::DepthBufferMode::TestAndWrite,
			//			Material::CullingMode::Back,
			//			(ui8)Material::ShadowsSettingsBitMask::ReceiveShadows
			//	};

			//	Rendering::Material* planeMaterial = Rendering::Material::Create("PlaneMaterial", shapeMaterialProperties);

			//	ResourceManagement::ResourceManager::AddResource(planeMaterial);

			//	planeMaterial->AssignShader();
			//	
			//	Rendering::PlaneMesh* plane = MemoryManager::New<Rendering::PlaneMesh>("Plane", AllocationType::FreeList_Renderer, planeMaterial, 100.0f, "Editor/PlaneMesh.mesh");

			//	ResourceManagement::ResourceManager::AddResource(plane);

			//	plane->SetUpPlane();

			//	ECS::ECSMeshComponent* planeMeshComponent = plane->GenerateMeshComponent(m_ecsManager, true);

			//	m_ecsManager->AddComponentToEntity(planeEntity, planeMeshComponent);
			//	m_ecsManager->AddComponentToEntity(planeEntity, planeTransformComponent);				
			//}

			//{//Trees setup	

			//	Model* treeModel = ResourceManagement::ResourceManager::AddResource<Model>("assets/models/tree/Lowpoly_tree_sample.obj", ResourceManagement::ResourceType::Model);				

			//	treeModel->SetUpModel();

			//	glm::vec3 position = glm::vec3(0.0f);

			//	for (ui32 i = 0; i < 2; ++i)
			//	{
			//		for (ui32 j = 0; j < treeModel->GetSubMeshes().Size(); ++j)
			//		{					
			//			ECS::ECSEntityID treeEntity = m_ecsManager->AddEntity(("Tree" + std::to_string(i) + std::to_string(j)).c_str());

			//			ECS::ECSTransformComponent* treeTransformComponent = m_ecsManager->AddComponent<ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform,
			//				position,
			//				MathUtils::Vector3Zero,
			//				glm::vec3(5.0f));
			//		
			//			ECS::ECSMeshComponent* treeMeshComponent = treeModel->GetSubMeshes()[j]->GenerateMeshComponent(m_ecsManager, true);
			//			
			//			m_ecsManager->AddComponentToEntity(treeEntity, treeMeshComponent);
			//			m_ecsManager->AddComponentToEntity(treeEntity, treeTransformComponent);
			//		}

			//		position += glm::vec3(-15.0f, 0, 15.0f);
			//	}
			//}

			//{//Characters setup	

			//	Model* character =  ResourceManagement::ResourceManager::AddResource<Model>("assets/models/creature/creature.fbx", ResourceManagement::ResourceType::Model);				

			//	character->SetUpModel();

			//	glm::vec3 position = glm::vec3(0.0f);

			//	for (ui32 i = 0; i < 2; ++i)
			//	{
			//		for (ui32 j = 0; j < character->GetSubMeshes().Size(); ++j)
			//		{	
			//			ECS::ECSEntityID characterEntity = m_ecsManager->AddEntity(("Character" + std::to_string(i) + std::to_string(j)).c_str());

			//			ECS::ECSTransformComponent* characterTransformComponent = m_ecsManager->AddComponent<ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform,
			//				position,
			//				MathUtils::Vector3Zero,
			//				glm::vec3(0.5f));

			//			ECS::ECSMeshComponent* characterMeshComponent = character->GetSubMeshes()[j]->GenerateMeshComponent(m_ecsManager,false);
			//			ECS::ECSSkeletonComponent* characterSkeletonComponent = character->GetSubMeshes()[j]->GenerateSkeletonComponent(m_ecsManager);

			//			const std::string animatorName = STRINGID_TO_STRING(character->GetModelFolderPath()) + "Animator.anim";

			//			StringId animatorID = STRING_TO_STRINGID(animatorName);
			//			StringId initialAnimation = Animation::AnimationManager::GetAnimator(animatorID)->GetInitialAnimationID();

			//			ECS::ECSAnimationComponent* characterAnimationComponent = m_ecsManager->AddComponent<ECS::ECSAnimationComponent>(ECS::ECSComponentType::Animation, animatorID, initialAnimation);

			//			m_ecsManager->AddComponentToEntity(characterEntity, characterTransformComponent);
			//			m_ecsManager->AddComponentToEntity(characterEntity, characterSkeletonComponent);
			//			m_ecsManager->AddComponentToEntity(characterEntity, characterMeshComponent);
			//			m_ecsManager->AddComponentToEntity(characterEntity, characterAnimationComponent);
			//		}

			//		position += glm::vec3(-150.0f, 0, 0.0f);
			//	}
			//}

			//END ECS setup			
			
			m_sceneHierarchyPanel.SetSceneContext(SceneManagement::SceneManager::GetMainScene());
			m_entityPropertiesPanel.SetHierarchyPanel(&m_sceneHierarchyPanel);						
			m_editorViewportPanel.SetHierarchyPanel(&m_sceneHierarchyPanel);
			m_editorViewportPanel.SetupViewportCamera();
		}

		void BaldLionEditorLayer::OnDetach()
		{
			SceneManagement::SceneManager::Stop();			
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
		}

		void BaldLionEditorLayer::OnEvent(Event& e)
		{
			BaldLion::EventDispatcher dispatcher(e);
			
			dispatcher.Dispatch<BaldLion::WindowResizeEvent>(BL_BIND_FUNCTION(BaldLionEditorLayer::OnWindowResizeEvent));
			dispatcher.Dispatch<BaldLion::KeyPressedEvent>(BL_BIND_FUNCTION(BaldLionEditorLayer::OnKeyPressedEvent));
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

	}
}