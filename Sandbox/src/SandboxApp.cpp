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
		: BaldLion::Layer("Example"), m_cameraMovement(glm::vec3(0,0,0)), m_squareColor(0.8f, 0.2f, 0.3f, 1.0f)
	{
		m_cameraMovementSpeed = 1.0f;
		m_cameraRotationSpeed = 10.0f;

		m_cameraYawRotation = 0.0f;
		m_cameraPitchRotation = 0.0f;

		m_prevX = BaldLion::Input::GetMouseX();
		m_prevY = BaldLion::Input::GetMouseY();

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

		m_textureShader = BaldLion::Shader::Create("assets/shaders/Texture.glsl");

		m_texture = BaldLion::Texture2D::Create("assets/textures/checkerboard.png");
		m_textureAlpha = BaldLion::Texture2D::Create("assets/textures/logo.png");

		int slot = 0;

		std::dynamic_pointer_cast<BaldLion::OpenGLShader>(m_textureShader)->Bind();
		std::dynamic_pointer_cast<BaldLion::OpenGLShader>(m_textureShader)->SetUniform("u_texture", BaldLion::ShaderDataType::Int, &slot);

		m_camera = BaldLion::ProjectionCamera(glm::vec3(0, 0, 2), 640.0f, 420.0f, 0.1f, 100.0f);
	}

	virtual void OnUpdate(BaldLion::TimeStep timeStep) override
	{
		HandleCameraMovement(timeStep);

		BaldLion::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		BaldLion::RenderCommand::Clear();

		BaldLion::Renderer::BeginScene(m_camera, glm::vec3(0, 0, 0));

		m_texture->Bind();
		std::dynamic_pointer_cast<BaldLion::OpenGLShader>(m_textureShader)->Bind();
		std::dynamic_pointer_cast<BaldLion::OpenGLShader>(m_textureShader)->SetUniform("u_baseColor", BaldLion::ShaderDataType::Float4, &(m_squareColor));

		BaldLion::Renderer::Submit(m_squareVertexArray, m_textureShader);

		m_textureAlpha->Bind();		
		BaldLion::Renderer::Submit(m_squareVertexArray, m_textureShader);

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

	void HandleCameraMovement(float deltaTime)
	{
		m_cameraMovement = glm::vec3(0, 0, 0);
				 
		if (BaldLion::Input::IsKeyPressed(BL_KEY_W))
			m_cameraMovement -= m_camera.GetForwardDirection() * deltaTime;
		else if (BaldLion::Input::IsKeyPressed(BL_KEY_S))
			m_cameraMovement += m_camera.GetForwardDirection() * deltaTime;

		if (BaldLion::Input::IsKeyPressed(BL_KEY_A))
			m_cameraMovement -= m_camera.GetRightDirection() * deltaTime;
		else if (BaldLion::Input::IsKeyPressed(BL_KEY_D))
			m_cameraMovement += m_camera.GetRightDirection() * deltaTime;

		if (m_cameraMovement != glm::vec3(0, 0, 0))
			m_camera.SetPosition(m_camera.GetPosition() + m_cameraMovement);

		if (BaldLion::Input::IsMouseButtonPress(BL_MOUSE_BUTTON_2))
		{
			float deltaX = BaldLion::Input::GetMouseX() - m_prevX;
			float deltaY = BaldLion::Input::GetMouseY() - m_prevY;

			m_cameraYawRotation -= deltaX * m_cameraRotationSpeed * deltaTime;
			m_cameraPitchRotation -= deltaY * m_cameraRotationSpeed * deltaTime;
			m_camera.SetRotation(m_cameraPitchRotation, m_cameraYawRotation);
		}

		m_prevX = BaldLion::Input::GetMouseX();
		m_prevY = BaldLion::Input::GetMouseY();

	}

private:
	BaldLion::Ref<BaldLion::Shader> m_textureShader;
	BaldLion::Ref<BaldLion::VertexArray> m_squareVertexArray;
	BaldLion::Ref<BaldLion::Texture2D> m_texture, m_textureAlpha;

	BaldLion::ProjectionCamera m_camera;

	glm::vec3 m_cameraMovement;
	glm::vec4 m_squareColor;

	float m_cameraPitchRotation;
	float m_cameraYawRotation;

	float m_prevX;
	float m_prevY;
	
	float m_cameraRotationSpeed;
	float m_cameraMovementSpeed;
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