#include <BaldLion.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "BaldLion/Rendering/Platform/OpenGL/OpenGLShader.h"
#include "ImGui/imgui.h"

using namespace BaldLion;
using namespace BaldLion::Rendering;

class RendererTestLayer : public BaldLion::Layer
{
public:
	RendererTestLayer(uint32_t width, uint32_t height)
		: BaldLion::Layer("Example"), m_emissiveColor(1.0f), m_diffuseColor(1.0f), m_specularColor(1.0f), m_shininess(32.0f)
	{
		
		m_model = CreateRef<AnimatedModel>("assets/creature/creature.fbx");

		m_model->SetUpModel();
		m_cameraController = ProjectionCameraController(glm::vec3(0, 0, 250), (float)width, (float)height, 0.1f, 5000.0f, 100.0f);

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

	virtual void OnUpdate(BaldLion::TimeStep timeStep) override
	{
		BL_PROFILE_FUNCTION();

		{
			BL_PROFILE_SCOPE("CameraController::OnUpdate")
			m_cameraController.OnUpdate(timeStep);
		}

		{
			BL_PROFILE_SCOPE("Renderer::BeginScene")
			Renderer::BeginScene(m_cameraController.GetCamera(), directionalLight, pointLights);
		}
		
		{
			BL_PROFILE_SCOPE("Renderer::Draw")
			m_model->Draw();
			Renderer::EndScene();
		}
	}

	virtual void OnImGuiRender() override 
	{
		BL_PROFILE_FUNCTION();
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

	virtual void OnEvent(BaldLion::Event& event) override
	{
		BaldLion::EventDispatcher dispatcher(event);		
		dispatcher.Dispatch<BaldLion::WindowResizeEvent>(std::bind(&RendererTestLayer::OnWindowResize, this, std::placeholders::_1));
	}

	bool OnWindowResize(BaldLion::WindowResizeEvent& e)
	{		
		uint32_t width = e.GetWidth();
		uint32_t height = e.GetHeight();
		
		m_cameraController.GetCamera()->SetWidth((float)width);
		m_cameraController.GetCamera()->SetHeight((float)height);

		Renderer::OnWindowResize(width, height);		

		return true;
	}

private:

	Ref<AnimatedModel> m_model;
	ProjectionCameraController m_cameraController;

	DirectionalLight directionalLight;
	std::vector<PointLight> pointLights;

	glm::vec3 m_emissiveColor;
	glm::vec3 m_diffuseColor;
	glm::vec3 m_specularColor;

	float m_shininess;
};

class Sandbox : public Application
{
public:
	Sandbox()
	{		
		PushLayer(new RendererTestLayer(GetWindow().GetWidth(),GetWindow().GetHeight()));
	}

	~Sandbox()
	{

	}
};

BaldLion::Application* BaldLion::CreateApplication()
{
	return new Sandbox();
}