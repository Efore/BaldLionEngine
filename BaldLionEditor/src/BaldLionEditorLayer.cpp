#include "BaldLionEditorLayer.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include "BaldLion/Rendering/Platform/OpenGL/OpenGLShader.h"
#include "ImGui/imgui.h"

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
			
			//ECS setup

			m_ecsManager = MemoryManager::New<ECS::ECSManager>("ECS Manager", AllocationType::FreeList_ECS);

			{//Camera setup

				ECS::ECSEntityID cameraEntity = STRING_TO_STRINGID("Main Camera");

				m_ecsManager->AddEntity(cameraEntity);

				ECS::ECSTransformComponent* cameraTransformComponent = m_ecsManager->AddComponent<ECS::ECSTransformComponent>(ECS::ECSComponentID::Transform,
					glm::vec3(0, 5, 10),
					MathUtils::QuaternionIdentity,
					glm::vec3(1.0f));

				ECS::ECSProjectionCameraComponent* projectionCameraComponent = m_ecsManager->AddComponent<ECS::ECSProjectionCameraComponent>(ECS::ECSComponentID::ProjectionCamera,
					(float)Application::GetInstance().GetWindow().GetWidth(),
					(float)Application::GetInstance().GetWindow().GetHeight(),
					0.1f,
					50000.0f,
					50.0f,
					10.f);

				m_ecsManager->AddComponentToEntity(cameraEntity, cameraTransformComponent);
				m_ecsManager->AddComponentToEntity(cameraEntity, projectionCameraComponent);

				ECS::SingletonComponents::ECSProjectionCameraSingleton::Init();
				ECS::SingletonComponents::ECSProjectionCameraSingleton::SetMainCamera(projectionCameraComponent, cameraTransformComponent);
			}

			{//Directional Light setup

				ECS::ECSEntityID directionalLight = STRING_TO_STRINGID("Directional Light");

				m_ecsManager->AddEntity(directionalLight);
				ECS::ECSDirectionalLightComponent* directionalLightComponent = m_ecsManager->AddComponent<ECS::ECSDirectionalLightComponent>(
					ECS::ECSComponentID::DirectionalLight,
					glm::vec3(0.0f),
					glm::vec3(1.0f),
					glm::vec3(0.2f),
					glm::vec3(-1.0f, -1.0f, -1.0f));

				m_ecsManager->AddComponentToEntity(directionalLight, directionalLightComponent);

				ECS::SingletonComponents::ECSLightSingleton::SetDirectionalLight(directionalLightComponent);
			}

			{//Plane setup

				ECS::ECSEntityID planeEntity = STRING_TO_STRINGID("Plane");

				m_ecsManager->AddEntity(planeEntity);

				ECS::ECSTransformComponent* planeTransformComponent = m_ecsManager->AddComponent<ECS::ECSTransformComponent>(ECS::ECSComponentID::Transform,
					glm::vec3(0, 0, 0),
					MathUtils::QuaternionIdentity,
					glm::vec3(10.0f, 1.0f, 10.0f));

				Texture* gridTexture = Renderer::GetTextureLibrary().Load("assets/textures/TextureGrid.png", TextureType::Texture2d);
				gridTexture->SetWrapMode(WrapMode::Repeat, WrapMode::Repeat);

				Rendering::Material::MaterialProperties shapeMaterialProperties{
						STRING_TO_STRINGID("assets/shaders/baseLit.glsl"),
						glm::vec3(1.0f),
						glm::vec3(1.0f),
						glm::vec3(1.0f),
						glm::vec3(1.0f),
						32.0f,
						nullptr,
						gridTexture,
						nullptr,
						nullptr,
						nullptr,
						Material::BlendMode::None,
						Material::DepthBufferMode::TestAndWrite,
						Material::CullingMode::Back,
						(ui8)Material::ShadowsSettingsBitMask::ReceiveShadows
				};

				Rendering::Material* shapeMaterial = Rendering::MaterialLibrary::Load("PlaneMaterial", &shapeMaterialProperties);
				shapeMaterial->AssignShader();
				
				Rendering::PlaneMesh* plane = MemoryManager::New<Rendering::PlaneMesh>("Plane", AllocationType::FreeList_Renderer, shapeMaterial, 100.0f);
				plane->SetUpPlane();

				ECS::ECSMeshComponent* planeMeshComponent = plane->GenerateMeshComponent(m_ecsManager, true);

				m_ecsManager->AddComponentToEntity(planeEntity, planeMeshComponent);
				m_ecsManager->AddComponentToEntity(planeEntity, planeTransformComponent);				
			}

			{//Trees setup	

				Model* treeModel = MemoryManager::New<Rendering::Model>("Tree Model ", AllocationType::FreeList_Renderer, "assets/models/tree/Lowpoly_tree_sample.obj");
				glm::vec3 position = glm::vec3(0.0f);
				for (ui32 i = 0; i < 2; ++i)
				{
					treeModel->SetUpModel();

					for (ui32 j = 0; j < treeModel->GetSubMeshes().Size(); ++j)
					{
						ECS::ECSEntityID treeEntity = STRING_TO_STRINGID(("Tree" + std::to_string(i) + std::to_string(j)).c_str());
						m_ecsManager->AddEntity(treeEntity);

						ECS::ECSTransformComponent* treeTransformComponent = m_ecsManager->AddComponent<ECS::ECSTransformComponent>(ECS::ECSComponentID::Transform,
							position,
							MathUtils::QuaternionIdentity,
							glm::vec3(5.0f));
					
						ECS::ECSMeshComponent* treeMeshComponent = treeModel->GetSubMeshes()[j]->GenerateMeshComponent(m_ecsManager, true);

						m_ecsManager->AddComponentToEntity(treeEntity, treeMeshComponent);
						m_ecsManager->AddComponentToEntity(treeEntity, treeTransformComponent);
					}

					position += glm::vec3(-15.0f, 0, 15.0f);
				}
			}

			{//Characters setup	

				Model* character = MemoryManager::New<Rendering::Model>("Character Model", AllocationType::FreeList_Renderer, "assets/models/creature/creature.fbx");

				character->SetUpModel();

				glm::vec3 position = glm::vec3(0.0f);

				for (ui32 i = 0; i < 2; ++i)
				{
					for (ui32 j = 0; j < character->GetSubMeshes().Size(); ++j)
					{
						ECS::ECSEntityID characterEntity = STRING_TO_STRINGID(("Character" + std::to_string(i) + std::to_string(j)).c_str());
						m_ecsManager->AddEntity(characterEntity);

						ECS::ECSTransformComponent* characterTransformComponent = m_ecsManager->AddComponent<ECS::ECSTransformComponent>(ECS::ECSComponentID::Transform,
							position,
							MathUtils::QuaternionIdentity,
							glm::vec3(0.5f));

						ECS::ECSMeshComponent* characterMeshComponent = character->GetSubMeshes()[j]->GenerateMeshComponent(m_ecsManager,false);
						ECS::ECSSkeletonComponent* characterSkeletonComponent = character->GetSubMeshes()[j]->GenerateSkeletonComponent(m_ecsManager);
						ECS::ECSAnimationComponent* characterAnimationComponent = m_ecsManager->AddComponent<ECS::ECSAnimationComponent>(ECS::ECSComponentID::Animation);

						characterAnimationComponent->animatorID = STRING_TO_STRINGID("Animator");
						characterAnimationComponent->currentAnimationID = STRING_TO_STRINGID("mixamo.com");

						m_ecsManager->AddComponentToEntity(characterEntity, characterTransformComponent);
						m_ecsManager->AddComponentToEntity(characterEntity, characterSkeletonComponent);
						m_ecsManager->AddComponentToEntity(characterEntity, characterMeshComponent);
						m_ecsManager->AddComponentToEntity(characterEntity, characterAnimationComponent);
					}

					position += glm::vec3(-150.0f, 0, 0.0f);
				}
			}

			{//Systems
				const ECS::ECSSignature cameraMovementSystemSignature = ECS::GenerateSignature(2, ECS::ECSComponentID::ProjectionCamera, ECS::ECSComponentID::Transform);

				ECS::ECSCameraMovementSystem* cameraMovementSystem = MemoryManager::New<ECS::ECSCameraMovementSystem>("ECS CameraMovementSystem", AllocationType::FreeList_ECS,
					"ECS CameraMovementSystem", cameraMovementSystemSignature, m_ecsManager);

				const ECS::ECSSignature renderSystemSignature = ECS::GenerateSignature(2, ECS::ECSComponentID::Mesh, ECS::ECSComponentID::Transform);

				ECS::ECSRenderSystem* renderSystem = MemoryManager::New<ECS::ECSRenderSystem>("ECS RenderSystem", AllocationType::FreeList_ECS,
					"ECS RenderSystem", renderSystemSignature, m_ecsManager);

				const ECS::ECSSignature animationSystemSignature = ECS::GenerateSignature(2, ECS::ECSComponentID::Animation, ECS::ECSComponentID::Skeleton);

				ECS::ECSAnimationSystem* animationSystem = MemoryManager::New<ECS::ECSAnimationSystem>("ECS AnimationSystem", AllocationType::FreeList_ECS, "ECS AnimationSystem", animationSystemSignature, m_ecsManager);

				m_ecsManager->AddSystem(cameraMovementSystem);
				m_ecsManager->AddSystem(animationSystem);
				m_ecsManager->AddSystem(renderSystem);
			}

			m_ecsManager->StartSystems();

			//END ECS setup



			//for (ui32 i = 0; i < 3; ++i)
			//{
			//	initialTransform = glm::translate(initialTransform, glm::vec3(15, 0, 15));
			//	auto model = MemoryManager::New<Rendering::Model>(std::string("Animated Model " + i).c_str(), AllocationType::FreeList_Renderer, "assets/models/creature/creature.fbx", glm::scale( initialTransform, glm::vec3(0.1f)));
			//	model->SetUpModel();
			//	//Renderer::RegisterModel(model);
			//}

			//initialTransform = glm::mat4(1.0f);
			//initialTransform = glm::scale(initialTransform, glm::vec3(5.0f));

				
		}

		void BaldLionEditorLayer::OnDetach()
		{
			m_ecsManager->StopSystems();
			MemoryManager::Delete(m_ecsManager);
		}

		void BaldLionEditorLayer::OnUpdate(TimeStep timeStep)
		{
			BL_PROFILE_FUNCTION();

			{	
				m_ecsManager->UpdateSystems(timeStep);
			}	
			
			//Waiting for animation jobs
			JobManagement::JobManager::WaitForJobs();
				
			Renderer::BeginScene();			
			Renderer::DrawScene();
			Renderer::EndScene();		
		}

		void BaldLionEditorLayer::OnImGuiRender(TimeStep timeStep)
		{
			BL_PROFILE_FUNCTION();

			RenderDockSpace();

			ImGui::Begin("Viewport");

			m_viewPortFocused = ImGui::IsWindowFocused();

			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			if (m_viewportSize != glm::vec2{ viewportPanelSize.x, viewportPanelSize.y })
			{
				m_viewportSize = { viewportPanelSize.x, viewportPanelSize.y };
				Renderer::GetFrameBuffer()->Resize((ui32)viewportPanelSize.x, (ui32)viewportPanelSize.y);
			}

			ImGui::Image((void*)Renderer::GetFrameBuffer()->GetColorAttachmentID(), ImVec2{ m_viewportSize.x , m_viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			ImGui::End();

			ImGui::Begin("Settings");

			glm::vec3 directionalLightAmbientColor = ECS::SingletonComponents::ECSLightSingleton::GetDirectionaLightAmbientColor();
			glm::vec3 directionalLightDiffuseColor = ECS::SingletonComponents::ECSLightSingleton::GetDirectionaLightDiffuseColor();
			glm::vec3 directionalLightSpecularColor = ECS::SingletonComponents::ECSLightSingleton::GetDirectionaLightSpecularColor();
			glm::vec3 directionalLightDirection = ECS::SingletonComponents::ECSLightSingleton::GetDirectionaLightDirection();

			ImGui::Text("Directional Light");
			ImGui::SliderFloat3("Light Direction", glm::value_ptr(directionalLightDirection), -1.0f, 1.0f);
			ImGui::ColorEdit3("Light Ambient Color", glm::value_ptr(directionalLightAmbientColor));
			ImGui::ColorEdit3("Light Diffuse Color", glm::value_ptr(directionalLightDiffuseColor));
			ImGui::ColorEdit3("Light Specular Color", glm::value_ptr(directionalLightSpecularColor));

			ECS::SingletonComponents::ECSLightSingleton::SetDirectionaLightAmbientColor(directionalLightAmbientColor);
			ECS::SingletonComponents::ECSLightSingleton::SetDirectionaLightDiffuseColor(directionalLightDiffuseColor);
			ECS::SingletonComponents::ECSLightSingleton::SetDirectionaLightSpecularColor(directionalLightSpecularColor);
			ECS::SingletonComponents::ECSLightSingleton::SetDirectionaLightDirection(directionalLightDirection);

			ImGui::End();

			ImGui::Begin("Memory");

			ImGui::Text("Total Memory: %zu", MemoryManager::GetMemorySize());
			ImGui::Text("Free List Main Allocator: %zu / %zu", MemoryManager::GetAllocatorUsedMemory(AllocationType::FreeList_Main), MemoryManager::GetAllocatorSize(AllocationType::FreeList_Main));
			ImGui::Text("Free List Renderer Allocator: %zu / %zu", MemoryManager::GetAllocatorUsedMemory(AllocationType::FreeList_Renderer), MemoryManager::GetAllocatorSize(AllocationType::FreeList_Renderer));
			ImGui::Text("Free List ECS Allocator: %zu / %zu", MemoryManager::GetAllocatorUsedMemory(AllocationType::FreeList_ECS), MemoryManager::GetAllocatorSize(AllocationType::FreeList_ECS));
			ImGui::Text("Linear Frame Allocator: %zu / %zu", MemoryManager::GetAllocatorUsedMemory(AllocationType::Linear_Frame), MemoryManager::GetAllocatorSize(AllocationType::Linear_Frame));
			ImGui::Text("Stack Scope Allocator: %zu / %zu", MemoryManager::GetAllocatorUsedMemory(AllocationType::Stack), MemoryManager::GetAllocatorSize(AllocationType::Stack));

			ImGui::End();

			ImGui::Begin("Rendering");
			ImGui::Text("Performance: %f", 1.0f / timeStep.GetSeconds());
			ImGui::Text("Draw calls: %zu", Renderer::GetRenderStats().drawCalls);
			ImGui::Text("Vertices: %zu", Renderer::GetRenderStats().vertices);
			ImGui::End();
		}

		void BaldLionEditorLayer::OnEvent(Event& e)
		{
			BaldLion::EventDispatcher dispatcher(e);
			dispatcher.Dispatch<BaldLion::WindowResizeEvent>(std::bind(&BaldLionEditorLayer::OnWindowResizeEvent, this, std::placeholders::_1));
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

			// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
			if (opt_flags & ImGuiDockNodeFlags_PassthruDockspace)
				window_flags |= ImGuiWindowFlags_NoBackground;

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
					// Disabling fullscreen would allow the window to be moved to the front of other windows, 
					// which we can't undo at the moment without finer window depth/z control.
					//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

					if (ImGui::MenuItem("Exit"))
						BaldLion::Application::GetInstance().Close();

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

	}
}