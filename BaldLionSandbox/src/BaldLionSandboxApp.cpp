#include <BaldLion.h>
#include <BaldLion/Core/EntryPoint.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "BaldLion/Rendering/Platform/OpenGL/OpenGLShader.h"
#include "ImGui/imgui.h"

using namespace BaldLion;
using namespace BaldLion::Rendering;
using namespace BaldLion::JobManagement;

class RendererTestLayer : public BaldLion::Layer
{
public:
	RendererTestLayer(ui32 width, ui32 height)		
	{
		
	}

	void OnAttach()
	{
		OPTICK_EVENT();

		m_animatedModels = DynamicArray<Rendering::AnimatedModel*>(AllocationType::FreeList_Renderer, 1);		

		BaldLion::Rendering::FramebufferSpecification fbSpec;
		fbSpec.Width = Application::GetInstance().GetWindow().GetWidth();
		fbSpec.Height = Application::GetInstance().GetWindow().GetHeight();

		m_frameBuffer = BaldLion::Rendering::Framebuffer::Create(fbSpec);

		ProjectionCameraManager::Init(glm::vec3(0, 0, 250), (float)fbSpec.Width, (float)fbSpec.Height, 0.1f, 50000.0f, 100.0f);

		glm::mat4 initialTransform = glm::mat4(1.0f);

		for (ui32 i = 0; i < 1; ++i)
		{
			auto model = MemoryManager::New<Rendering::AnimatedModel>(STRING_TO_ID("Animated Model"), AllocationType::FreeList_Renderer, "assets/creature/creature.fbx", initialTransform);
			model->SetUpModel();
			m_animatedModels.PushBack(model);

			initialTransform = glm::translate(initialTransform, glm::vec3(150, 0, 0));
		}

		m_directionalLight = {
			glm::vec3(-0.2f, -1.0f, -0.3f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f)
		};

	}

	virtual void OnUpdate(BaldLion::TimeStep timeStep) override
	{
		OPTICK_EVENT();

		{
			OPTICK_CATEGORY("CameraController::OnUpdate", Optick::Category::Camera);

			if (m_viewPortFocused)
				ProjectionCameraManager::OnUpdate(timeStep);
		}

		{
			OPTICK_CATEGORY("AnimationManager::KickOnUpdate", Optick::Category::Animation);
			/*Animation::AnimationManager::OnUpdate(timeStep);*/
			StringId animationUpdateTaskId = 0;
			Animation::AnimationManager::OnParallelUpdate(timeStep, animationUpdateTaskId);
			JobManager::SetJobDependency(animationUpdateTaskId);			
		}

		JobManagement::JobManager::WaitForJobs();
		{
			OPTICK_CATEGORY("Renderer::BeginScene", Optick::Category::Rendering);
			m_frameBuffer->Bind();
			Renderer::BeginScene(ProjectionCameraManager::GetCamera(), m_directionalLight);
		}

		{
			OPTICK_CATEGORY("Renderer::Draw", Optick::Category::Rendering);
			for (ui32 i = 0; i < m_animatedModels.Size(); ++i)
			{
				m_animatedModels[i]->Draw();
			}
			Renderer::EndScene();
			m_frameBuffer->Unbind();
		}
	}

	virtual void OnImGuiRender(TimeStep timeStep) override
	{
		OPTICK_EVENT();
		ImGui::Begin("Settings");

		ImGui::Text("Directional Light");
		ImGui::SliderFloat3("Light Direction", glm::value_ptr(m_directionalLight.direction), -300.0f, 300.0f);
		ImGui::ColorEdit3("Light Ambient Color", glm::value_ptr(m_directionalLight.ambientColor));
		ImGui::ColorEdit3("Light Diffuse Color", glm::value_ptr(m_directionalLight.diffuseColor));
		ImGui::ColorEdit3("Light Specular Color", glm::value_ptr(m_directionalLight.specularColor));

		for (ui32 i = 0; i < m_pointLights.Size(); ++i)
		{
			ImGui::Text(("Point Light " + std::to_string(i)).c_str());
			ImGui::SliderFloat3(("Light Position " + std::to_string(i)).c_str(), glm::value_ptr(m_pointLights[i].position), -300.0f, 300.0f);
			ImGui::SliderFloat(("Constant " + std::to_string(i)).c_str(), &(m_pointLights[i].constant), 0.001f, 1.0f);
			ImGui::SliderFloat(("Linear " + std::to_string(i)).c_str(), &(m_pointLights[i].linear), 0.001f, 1.0f);
			ImGui::SliderFloat(("Quadratic " + std::to_string(i)).c_str(), &(m_pointLights[i].quadratic), 0.001f, 1.0f);
			ImGui::ColorEdit3(("LP Ambient Color " + std::to_string(i)).c_str(), glm::value_ptr(m_pointLights[i].ambientColor));
			ImGui::ColorEdit3(("LP Diffuse Color " + std::to_string(i)).c_str(), glm::value_ptr(m_pointLights[i].diffuseColor));
			ImGui::ColorEdit3(("LP Specular Color " + std::to_string(i)).c_str(), glm::value_ptr(m_pointLights[i].specularColor));
		}

		ImGui::End();
	}

	void OnDetach()
	{
		m_pointLights.Clear();

		for (ui32 i = 0; i < m_animatedModels.Size(); ++i)
		{
			MemoryManager::DeleteNoDestructor(m_animatedModels[i]);
		}
		m_animatedModels.Clear();

		MemoryManager::Delete(m_frameBuffer);
	}


	virtual void OnEvent(BaldLion::Event& event) override
	{
		BaldLion::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<BaldLion::WindowResizeEvent>(std::bind(&RendererTestLayer::OnWindowResize, this, std::placeholders::_1));
	}

	bool OnWindowResize(BaldLion::WindowResizeEvent& e)
	{
		ui32 width = e.GetWidth();
		ui32 height = e.GetHeight();

		ProjectionCameraManager::GetCamera()->SetWidth((float)width);
		ProjectionCameraManager::GetCamera()->SetHeight((float)height);

		Renderer::OnWindowResize(width, height);

		return true;
	}

private:

	DynamicArray<Rendering::AnimatedModel*> m_animatedModels;
	Rendering::Framebuffer* m_frameBuffer;

	glm::vec2 m_viewportSize;
	bool m_viewPortFocused;

	DirectionalLight m_directionalLight;
	DynamicArray<PointLight> m_pointLights;
};

class Sandbox : public Application
{
public:
	~Sandbox()
	{
		Animation::AnimationManager::Stop();
	}

	static Application& GetInstance(const std::string& applicationName)
	{
		if (s_instance == nullptr)
			s_instance = new Sandbox(applicationName);

		return *s_instance;
	}

private:

	Sandbox(const std::string& applicationName) : Application(applicationName)
	{
		Animation::AnimationManager::Init();
		PushLayer(new RendererTestLayer(GetWindow().GetWidth(), GetWindow().GetHeight()));
	}
};

BaldLion::Application& BaldLion::CreateApplication(const std::string& applicationName)
{
	return Sandbox::GetInstance(applicationName);
}