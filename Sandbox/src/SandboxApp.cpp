#include <BaldLion.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"
#include "ImGui/imgui.h"


class ExampleLayer : public BaldLion::Layer
{
public:
	ExampleLayer()
		: BaldLion::Layer("Example"), m_squareColor(0.8f, 0.2f, 0.3f, 1.0f)
	{
		//square definition
		m_squareVertexArray = BaldLion::VertexArray::Create();

		float sqrVertices[5 * 4] = {
		   -0.75f, -0.75f, 0.0f, 0.0f, 0.0f,		//index 0
			0.75f, -0.75f, 0.0f, 1.0f, 0.0f, 		//index 1
			0.75f,  0.75f, 0.0f, 1.0f, 1.0f,			//index 2
		   -0.75f,  0.75f, 0.0f, 0.0f, 1.0f			//index 3
		};

		BaldLion::Ref<BaldLion::VertexBuffer> squareVB;
		squareVB = BaldLion::VertexBuffer::Create(sqrVertices, sizeof(sqrVertices));
		squareVB->SetLayout({
			{ BaldLion::ShaderDataType::Float3, "a_position"},
			{ BaldLion::ShaderDataType::Float2, "a_TexCoord"}
		});

		m_squareVertexArray->AddVertexBuffer(squareVB);


		uint32_t sqrIndices[6] = { 0, 1, 2, 2, 3, 0 };

		BaldLion::Ref<BaldLion::IndexBuffer> squareIB;
		squareIB = (BaldLion::IndexBuffer::Create(sqrIndices, sizeof(sqrIndices) / sizeof(uint32_t)));
		m_squareVertexArray->AddIndexBuffer(squareIB);

		auto textureShader = BaldLion::Renderer::GetShaderLibrary().Load("assets/shaders/Texture.glsl");

		m_texture = BaldLion::Texture2D::Create("assets/textures/checkerboard.png");
		m_textureAlpha = BaldLion::Texture2D::Create("assets/textures/logo.png");

		int slot = 0;

		std::dynamic_pointer_cast<BaldLion::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<BaldLion::OpenGLShader>(textureShader)->SetUniform("u_texture", BaldLion::ShaderDataType::Int, &slot);

		m_cameraController = BaldLion::ProjectionCameraController(glm::vec3(0, 0, 2), 640.0f, 420.0f, 0.1f, 100.0f);
	}

	virtual void OnUpdate(BaldLion::TimeStep timeStep) override
	{
		m_cameraController.OnUpdate(timeStep);

		BaldLion::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		BaldLion::RenderCommand::Clear();

		BaldLion::Renderer::BeginScene(m_cameraController.GetCamera(), glm::vec3(0, 0, 0));

		auto textureShader = BaldLion::Renderer::GetShaderLibrary().Get("Texture");

		m_texture->Bind();
		std::dynamic_pointer_cast<BaldLion::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<BaldLion::OpenGLShader>(textureShader)->SetUniform("u_baseColor", BaldLion::ShaderDataType::Float4, &(m_squareColor));

		BaldLion::Renderer::Submit(m_squareVertexArray, textureShader);

		m_textureAlpha->Bind();		
		BaldLion::Renderer::Submit(m_squareVertexArray, textureShader);

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

	}

private:

	BaldLion::Ref<BaldLion::VertexArray> m_squareVertexArray;
	BaldLion::Ref<BaldLion::Texture2D> m_texture, m_textureAlpha;

	BaldLion::ProjectionCameraController m_cameraController;

	glm::vec4 m_squareColor;
};

class Sandbox : public BaldLion::Application
{
public:
	Sandbox()
	{		
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}
};

BaldLion::Application* BaldLion::CreateApplication()
{
	return new Sandbox();
}