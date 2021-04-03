#include "BaldLionEditorLayer.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

			BaldLion::Rendering::FramebufferSpecification fbSpec;
			fbSpec.Width = Application::GetInstance().GetWindow().GetWidth();
			fbSpec.Height = Application::GetInstance().GetWindow().GetHeight();

			m_frameBuffer = BaldLion::Rendering::Framebuffer::Create(fbSpec);
			
			ProjectionCameraManager::Init(glm::vec3(0, 0, 0), (float)fbSpec.Width, (float)fbSpec.Height, 0.1f, 50000.0f, 500.0f);			

			glm::mat4 initialTransform = glm::mat4(1.0f);

			for (ui32 i = 0; i < 3; ++i)
			{
				auto model = MemoryManager::New<Rendering::AnimatedModel>(STRING_TO_ID("Animated Model " + i), AllocationType::FreeList_Renderer, "assets/models/creature/creature.fbx", initialTransform);
				model->SetUpModel();
				Renderer::SubscribeModel(model);
				initialTransform = glm::translate(initialTransform, glm::vec3(150, 0, 0));
			}

			initialTransform = glm::mat4(1.0f);
			initialTransform = glm::scale(initialTransform, glm::vec3(50.0f));
			initialTransform = glm::translate(initialTransform, glm::vec3(0, 0, 0));

			for (ui32 i = 0; i < 300; ++i)
			{
				auto model = MemoryManager::New<Rendering::Model>(STRING_TO_ID("Static Model " + i), AllocationType::FreeList_Renderer, "assets/models/tree/Lowpoly_tree_sample.obj", initialTransform);
				model->SetUpModel();
				Renderer::SubscribeModel(model);
				if (i > 0 && i % 60 == 0)
				{
					initialTransform = glm::translate(initialTransform, glm::vec3(-15.0f * 60, 0, 15.0f * (i / 60)));
				}
				else
				{
					initialTransform = glm::translate(initialTransform, glm::vec3(15.0f, 0, 0.0f));
				}				
			}

			m_directionalLight = {
				glm::vec3(-0.2f, -1.0f, -0.3f),
				glm::vec3(1.0f, 1.0f, 1.0f),
				glm::vec3(1.0f, 1.0f, 1.0f),
				glm::vec3(1.0f, 1.0f, 1.0f)
			};
			
		}

		void BaldLionEditorLayer::OnDetach()
		{
			MemoryManager::Delete(m_frameBuffer);
		}

		void BaldLionEditorLayer::OnUpdate(TimeStep timeStep)
		{
			BL_PROFILE_FUNCTION();

			{
				BL_PROFILE_SCOPE("CameraController::OnUpdate");
				
				if (m_viewPortFocused)
					ProjectionCameraManager::OnUpdate(timeStep);
			}

			{
				BL_PROFILE_SCOPE("AnimationManager::KickOnUpdate");
				/*Animation::AnimationManager::OnUpdate(timeStep);*/
				StringId animationUpdateTaskId = 0;
				Animation::AnimationManager::OnParallelUpdate(timeStep, animationUpdateTaskId);		
			}			
			
			JobManagement::JobManager::WaitForJobs();
			{
				BL_PROFILE_SCOPE("Renderer::BeginScene");
				m_frameBuffer->Bind();
				Renderer::BeginScene(ProjectionCameraManager::GetCamera(), m_directionalLight);
			}
			{

				BL_PROFILE_SCOPE("Renderer::Draw static models");
				Renderer::DrawScene(ProjectionCameraManager::GetCamera());
				Renderer::EndScene();
				m_frameBuffer->Unbind();
			}
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
				m_frameBuffer->Resize((ui32)viewportPanelSize.x, (ui32)viewportPanelSize.y);
			}

			ImGui::Image((void*)m_frameBuffer->GetColorAttachmentID(), ImVec2{ m_viewportSize.x , m_viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			ImGui::End();

			ImGui::Begin("Settings");

			ImGui::Text("Directional Light");
			ImGui::SliderFloat3("Light Direction", glm::value_ptr(m_directionalLight.direction), -1.0f, 1.0f);
			ImGui::ColorEdit3("Light Ambient Color", glm::value_ptr(m_directionalLight.ambientColor));
			ImGui::ColorEdit3("Light Diffuse Color", glm::value_ptr(m_directionalLight.diffuseColor));
			ImGui::ColorEdit3("Light Specular Color", glm::value_ptr(m_directionalLight.specularColor));

			ImGui::End();

			ImGui::Begin("Memory");

			ImGui::Text("Total Memory: %zu", MemoryManager::GetMemorySize());
			ImGui::Text("Free List Main Allocator: %zu / %zu", MemoryManager::GetAllocatorUsedMemory(AllocationType::FreeList_Main), MemoryManager::GetAllocatorSize(AllocationType::FreeList_Main));
			ImGui::Text("Free List Renderer Allocator: %zu / %zu", MemoryManager::GetAllocatorUsedMemory(AllocationType::FreeList_Renderer), MemoryManager::GetAllocatorSize(AllocationType::FreeList_Renderer));
			ImGui::Text("Linear Frame Allocator: %zu / %zu", MemoryManager::GetAllocatorUsedMemory(AllocationType::Linear_Frame), MemoryManager::GetAllocatorSize(AllocationType::Linear_Frame));
			ImGui::Text("Stack Scope Allocator: %zu / %zu", MemoryManager::GetAllocatorUsedMemory(AllocationType::Stack), MemoryManager::GetAllocatorSize(AllocationType::Stack));

			ImGui::End();

			ImGui::Begin("Rendering");
			ImGui::Text("Performance: %f", 1.0f / timeStep.GetSeconds());
			ImGui::Text("Draw calls: %zu", Renderer::GetRenderStats().drawCalls);
			ImGui::Text("Vertices: %zu", Renderer::GetRenderStats().vertices);
			ImGui::End();

			Renderer::GetRenderStats().drawCalls = 0;
			Renderer::GetRenderStats().vertices = 0;
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

			ProjectionCameraManager::GetCamera()->SetWidth((float)width);
			ProjectionCameraManager::GetCamera()->SetHeight((float)height);

			Renderer::OnWindowResize(width, height);

			return true;
		}

	}
}