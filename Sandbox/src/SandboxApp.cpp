#include <BaldLion.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"
#include "ImGui/imgui.h"


class RendererTestLayer : public BaldLion::Layer
{
public:
	RendererTestLayer(uint32_t width, uint32_t height)
		: BaldLion::Layer("Example"), m_emissiveColor(1.0f), m_diffuseColor(1.0f), m_specularColor(1.0f), m_lightPosition(0.0f,200.0f,0.0f), m_shininess(32.0f)
	{

		m_model = std::make_shared<BaldLion::Model>("assets/models/model.obj");
		m_cameraController = BaldLion::ProjectionCameraController(glm::vec3(0, 0, 250), (float)width, (float)height, 0.1f, 500.0f, 100.0f);
	}

	virtual void OnUpdate(BaldLion::TimeStep timeStep) override
	{
		m_cameraController.OnUpdate(timeStep);

		BaldLion::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		BaldLion::RenderCommand::Clear();

		BaldLion::Renderer::BeginScene(m_cameraController.GetCamera(),m_lightPosition);

		for (auto submesh : m_model->GetSubMeshes())
		{
			submesh.GetMaterial()->SetEmissiveColor(m_emissiveColor);
			submesh.GetMaterial()->SetDiffuseColor(m_diffuseColor);
			submesh.GetMaterial()->SetSpecularColor(m_specularColor);
			submesh.GetMaterial()->SetShininess(m_shininess);
		}

		m_model->Draw();

		BaldLion::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override 
	{
		ImGui::Begin("Settings");
		ImGui::SliderFloat3("Emissive Color", glm::value_ptr(m_emissiveColor), 0.0f, 5.0f);
		ImGui::ColorEdit3("Diffuse Color", glm::value_ptr(m_diffuseColor));
		ImGui::ColorEdit3("Specular Color", glm::value_ptr(m_specularColor));
		ImGui::SliderFloat("Shininess", &m_shininess, 1.0f, 300.0f);
		ImGui::SliderFloat3("Light Position", glm::value_ptr(m_lightPosition), -300.0f, 300.0f);
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

		BaldLion::Renderer::OnWindowResize(width, height);		

		return true;
	}

private:

	BaldLion::Ref<BaldLion::VertexArray> m_squareVertexArray;
	BaldLion::Ref<BaldLion::Texture2D> m_texture, m_textureAlpha;

	BaldLion::Ref<BaldLion::Model> m_model;
	BaldLion::ProjectionCameraController m_cameraController;

	glm::vec3 m_emissiveColor;
	glm::vec3 m_diffuseColor;
	glm::vec3 m_specularColor;
	glm::vec3 m_lightPosition;

	float m_shininess;
};

class Sandbox : public BaldLion::Application
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