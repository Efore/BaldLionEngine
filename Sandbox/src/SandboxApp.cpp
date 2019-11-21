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
		: BaldLion::Layer("Example"), m_squareColor(0.8f, 0.2f, 0.3f, 1.0f)
	{

		m_mesh = std::make_shared<BaldLion::Mesh>("assets/models/model.obj");
		m_mesh->SetUpMesh();
		m_cameraController = BaldLion::ProjectionCameraController(glm::vec3(0, 0, 2), (float)width, (float)height, 0.1f, 100.0f);
	}

	virtual void OnUpdate(BaldLion::TimeStep timeStep) override
	{
		m_cameraController.OnUpdate(timeStep);

		BaldLion::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		BaldLion::RenderCommand::Clear();

		BaldLion::Renderer::BeginScene(m_cameraController.GetCamera(), glm::vec3(0, 0, 0));

		m_mesh->Draw();

		BaldLion::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override 
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_squareColor));
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
		
		m_cameraController.GetCamera().SetWidth((float)width);
		m_cameraController.GetCamera().SetHeight((float)height);

		BaldLion::Renderer::OnWindowResize(width, height);		

		return true;
	}

private:

	BaldLion::Ref<BaldLion::VertexArray> m_squareVertexArray;
	BaldLion::Ref<BaldLion::Texture2D> m_texture, m_textureAlpha;

	BaldLion::Ref<BaldLion::Mesh> m_mesh;
	BaldLion::ProjectionCameraController m_cameraController;

	glm::vec4 m_squareColor;
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