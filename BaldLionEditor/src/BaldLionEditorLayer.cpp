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
			: Layer("BaldLion Editor Layer")
		{
			
		}

		void BaldLionEditorLayer::OnAttach()
		{
			BL_PROFILE_FUNCTION();

			BaldLion::Rendering::FramebufferSpecification fbSpec;
			fbSpec.Width = Application::GetInstance().GetWindow().GetWidth();
			fbSpec.Height = Application::GetInstance().GetWindow().GetHeight();

			m_frameBuffer = BaldLion::Rendering::Framebuffer::Create(fbSpec);

			glm::mat4 initialTransform = glm::mat4(1.0f);

			for (int i = 0; i < 1; ++i)
			{
				auto model = BaldLion::CreateRef<Rendering::AnimatedModel>("assets/creature/creature.fbx", initialTransform);
				model->SetUpModel();
				m_models.push_back(model);

				initialTransform = glm::translate(initialTransform, glm::vec3(150, 0, 0));
			}

			ProjectionCameraManager::GetInstance()->SetUpInitialValues(glm::vec3(0, 0, 250), (float)fbSpec.Width, (float)fbSpec.Height, 0.1f, 50000.0f, 100.0f);

			directionalLight = {
				glm::vec3(-0.2f, -1.0f, -0.3f),
				glm::vec3(1.0f, 1.0f, 1.0f),
				glm::vec3(1.0f, 1.0f, 1.0f),
				glm::vec3(1.0f, 1.0f, 1.0f)
			};

			pointLights.emplace_back(PointLight
			({
				glm::vec3(0.7f,  0.2f,  2.0f),
				1.0f,
				0.009f,
				0.032f,
				glm::vec3(1.0f, 1.0f, 1.0f),
				glm::vec3(0.8f, 0.8f, 0.8f),
				glm::vec3(1.0f, 1.0f, 1.0f)
				})
			);

			pointLights.emplace_back(PointLight
			({
				glm::vec3(0.7f,  0.2f,  2.0f),
				1.0f,
				0.009f,
				0.032f,
				glm::vec3(1.0f, 1.0f, 1.0f),
				glm::vec3(0.8f, 0.8f, 0.8f),
				glm::vec3(1.0f, 1.0f, 1.0f)
				})
			);
		}

		void BaldLionEditorLayer::OnDetach()
		{

		}

		void BaldLionEditorLayer::OnUpdate(TimeStep timeStep)
		{
			BL_PROFILE_FUNCTION();

			{
				BL_PROFILE_SCOPE("CameraController::OnUpdate");
				ProjectionCameraManager::GetInstance()->OnUpdate(timeStep);
			}

			{
				BL_PROFILE_SCOPE("AnimationManager::OnUpdate");
				Animation::AnimationManager::GetInstance()->OnUpdate(timeStep);
			}

			{
				BL_PROFILE_SCOPE("Renderer::BeginScene");
				m_frameBuffer->Bind();
				Renderer::BeginScene(ProjectionCameraManager::GetInstance()->GetCamera(), directionalLight, pointLights);
			}

			{
				BL_PROFILE_SCOPE("Renderer::Draw");
				for (const auto& model : m_models)
				{
					model->Draw();
				}
				Renderer::EndScene();
				m_frameBuffer->Unbind();
			}
		}

		void BaldLionEditorLayer::OnImGuiRender()
		{
			BL_PROFILE_FUNCTION();

			RenderDockSpace();

			ImGui::Begin("Viewport");

			ImGui::Image((void*)m_frameBuffer->GetColorAttachmentID(), ImVec2{ ImGui::GetWindowWidth(), ImGui::GetWindowHeight() }, ImVec2{ 0, 0 }, ImVec2{ 1, -1 });
			ImGui::End();

			ImGui::Begin("Settings");

			ImGui::Text("Directional Light");
			ImGui::SliderFloat3("Light Direction", glm::value_ptr(directionalLight.direction), -300.0f, 300.0f);
			ImGui::ColorEdit3("Light Ambient Color", glm::value_ptr(directionalLight.ambientColor));
			ImGui::ColorEdit3("Light Diffuse Color", glm::value_ptr(directionalLight.diffuseColor));
			ImGui::ColorEdit3("Light Specular Color", glm::value_ptr(directionalLight.specularColor));

			for (int i = 0; i < pointLights.size(); ++i)
			{
				ImGui::Text(("Point Light " + std::to_string(i)).c_str());
				ImGui::SliderFloat3(("Light Position " + std::to_string(i)).c_str(), glm::value_ptr(pointLights[i].position), -300.0f, 300.0f);
				ImGui::SliderFloat(("Constant " + std::to_string(i)).c_str(), &(pointLights[i].constant), 0.001f, 1.0f);
				ImGui::SliderFloat(("Linear " + std::to_string(i)).c_str(), &(pointLights[i].linear), 0.001f, 1.0f);
				ImGui::SliderFloat(("Quadratic " + std::to_string(i)).c_str(), &(pointLights[i].quadratic), 0.001f, 1.0f);
				ImGui::ColorEdit3(("LP Ambient Color " + std::to_string(i)).c_str(), glm::value_ptr(pointLights[i].ambientColor));
				ImGui::ColorEdit3(("LP Diffuse Color " + std::to_string(i)).c_str(), glm::value_ptr(pointLights[i].diffuseColor));
				ImGui::ColorEdit3(("LP Specular Color " + std::to_string(i)).c_str(), glm::value_ptr(pointLights[i].specularColor));
			}

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
			uint32_t width = e.GetWidth();
			uint32_t height = e.GetHeight();

			ProjectionCameraManager::GetInstance()->GetCamera()->SetWidth((float)width);
			ProjectionCameraManager::GetInstance()->GetCamera()->SetHeight((float)height);

			Renderer::OnWindowResize(width, height);

			return true;
		}

	}
}