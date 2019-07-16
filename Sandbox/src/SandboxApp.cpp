#include <BaldLion.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ExampleLayer : public BaldLion::Layer
{
public:
	ExampleLayer()
		: BaldLion::Layer("Example"), m_cameraMovement(glm::vec3(0,0,0))
	{
		m_cameraMovementSpeed = 1.0f;
		m_cameraRotationSpeed = 10.0f;

		m_cameraYawRotation = 0.0f;
		m_cameraPitchRotation = 0.0f;

		m_prevX = BaldLion::Input::GetMouseX();
		m_prevY = BaldLion::Input::GetMouseY();
		//triangle definition
		m_triangleVertexArray.reset(BaldLion::VertexArray::Create());

		float triangleVertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,		//index 0
			0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,		//index 1
			0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f		//index 2
		};

		std::shared_ptr<BaldLion::VertexBuffer> triangleVB;
		triangleVB.reset(BaldLion::VertexBuffer::Create(triangleVertices, sizeof(triangleVertices)));
		triangleVB->SetLayout({
			{ BaldLion::ShaderDataType::Float3, "a_position" },
			{ BaldLion::ShaderDataType::Float4, "a_color" }
		});
		m_triangleVertexArray->AddVertexBuffer(triangleVB);


		uint32_t triangleIndices[3] = { 0, 1, 2 };

		std::shared_ptr<BaldLion::IndexBuffer> triangleIB;
		triangleIB.reset(BaldLion::IndexBuffer::Create(triangleIndices, sizeof(triangleIndices) / sizeof(uint32_t)));
		m_triangleVertexArray->AddIndexBuffer(triangleIB);

		//square definition
		m_squareVertexArray.reset(BaldLion::VertexArray::Create());

		float sqrVertices[3 * 4] = {
			-0.75f, -0.75f, 0.0f,		//index 0
			0.75f, -0.75f, 0.0f, 		//index 1
			0.75f, 0.75f, 0.0f,			//index 2
			-0.75f, 0.75f, 0.0f			//index 3
		};

		std::shared_ptr<BaldLion::VertexBuffer> squareVB;
		squareVB.reset(BaldLion::VertexBuffer::Create(sqrVertices, sizeof(sqrVertices)));
		squareVB->SetLayout({
			{ BaldLion::ShaderDataType::Float3, "a_position"}
		});
		m_squareVertexArray->AddVertexBuffer(squareVB);


		uint32_t sqrIndices[6] = { 0, 1, 2, 2, 3, 0 };

		std::shared_ptr<BaldLion::IndexBuffer> squareIB;
		squareIB.reset(BaldLion::IndexBuffer::Create(sqrIndices, sizeof(sqrIndices) / sizeof(uint32_t)));
		m_squareVertexArray->AddIndexBuffer(squareIB);


		std::string vertexSource = R"(
			#version 330 core

			layout(location = 0) in vec3 a_position;		
			layout(location = 1) in vec4 a_color;				

			uniform mat4 u_viewProjection;  
			uniform mat4 u_transform;

			out vec3 v_position;
			out vec4 v_color;

			void main()
			{
				v_position = a_position + 0.5;
				v_color = a_color;
				gl_Position = u_viewProjection * u_transform * vec4(a_position, 1.0);
			}
		)";

		std::string fragmentSource = R"(
			#version 330 core

			layout(location = 0) out vec4 color;			

			in vec3 v_position;
			in vec4 v_color;
	
			void main()
			{
				color = v_color;
			}
		)";

		std::string vertexSource2 = R"(
			#version 330 core

			layout(location = 0) in vec3 a_position;		

			uniform mat4 u_viewProjection;  
			uniform mat4 u_transform;
		
			out vec3 v_position;			
			
			void main()
			{
				v_position = a_position + 0.5;						
				gl_Position = u_viewProjection * u_transform * vec4(a_position, 1.0);
			}
		)";

		std::string fragmentSource2 = R"(
			#version 330 core

			layout(location = 0) out vec4 color;			

			in vec3 v_position;			
	
			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";


		m_triangleShader.reset(new BaldLion::Shader(vertexSource, fragmentSource));
		m_squareShader.reset(new BaldLion::Shader(vertexSource2, fragmentSource2));

		m_camera.reset(new BaldLion::ProjectionCamera(glm::vec3(0, 0, 0), 640.0f, 420.0f, 0.1f, 100.0f));
	}

	virtual void OnUpdate(BaldLion::TimeStep timeStep) override
	{
		HandleCameraMovement(timeStep);

		BaldLion::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		BaldLion::RenderCommand::Clear();

		BaldLion::Renderer::BeginScene(m_camera, glm::vec3(0, 0, 0));

		BaldLion::Renderer::Submit(m_squareVertexArray, m_squareShader);
		BaldLion::Renderer::Submit(m_triangleVertexArray, m_triangleShader);

		BaldLion::Renderer::EndScene();
	}

	virtual void OnEvent(BaldLion::Event& event) override
	{

	}

private:

	void HandleCameraMovement(float deltaTime)
	{
		m_cameraMovement = glm::vec3(0, 0, 0);
				 
		if (BaldLion::Input::IsKeyPressed(BL_KEY_W))
			m_cameraMovement += glm::vec3(0.0f, 0.0f, -m_cameraMovementSpeed) * deltaTime;
		else if (BaldLion::Input::IsKeyPressed(BL_KEY_S))
			m_cameraMovement += glm::vec3(0.0f, 0.0f, m_cameraMovementSpeed) * deltaTime;

		if (BaldLion::Input::IsKeyPressed(BL_KEY_A))
			m_cameraMovement += glm::vec3(-m_cameraMovementSpeed, 0.0f, 0.0f) * deltaTime;
		else if (BaldLion::Input::IsKeyPressed(BL_KEY_D))
			m_cameraMovement += glm::vec3(m_cameraMovementSpeed, 0.0f, 0.0f) * deltaTime;

		if (m_cameraMovement != glm::vec3(0, 0, 0))
			m_camera->SetPosition(m_camera->GetPosition() + m_cameraMovement);

		if (BaldLion::Input::IsMouseButtonPress(BL_MOUSE_BUTTON_1))
		{
			float deltaX = BaldLion::Input::GetMouseX() - m_prevX;
			float deltaY = BaldLion::Input::GetMouseY() - m_prevY;

			m_cameraYawRotation -= deltaX * m_cameraRotationSpeed * deltaTime;
			m_cameraPitchRotation -= deltaY * m_cameraRotationSpeed * deltaTime;
			m_camera->SetRotation(m_cameraPitchRotation, m_cameraYawRotation);
		}

		m_prevX = BaldLion::Input::GetMouseX();
		m_prevY = BaldLion::Input::GetMouseY();

	}

private:
	std::shared_ptr<BaldLion::Shader> m_triangleShader;
	std::shared_ptr<BaldLion::Shader> m_squareShader;

	std::shared_ptr<BaldLion::VertexArray> m_triangleVertexArray;
	std::shared_ptr<BaldLion::VertexArray> m_squareVertexArray;

	std::shared_ptr<BaldLion::ProjectionCamera> m_camera;

	glm::vec3 m_cameraMovement;

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