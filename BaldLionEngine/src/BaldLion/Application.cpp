#include "blpch.h"
#include "Application.h"

#include "BaldLion/Log.h"

#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"

namespace BaldLion
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_instance = nullptr;

	Application::Application()
	{
		BL_CORE_ASSERT(!s_instance, "Application already exists");

		s_instance = this;

		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		m_imGuiLayer = new ImGuiLayer();
		PushOverlay(m_imGuiLayer);
	
		//triangle definition
		m_triangleVertexArray.reset(VertexArray::Create());
		
		float triangleVertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,		//index 0
			0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,		//index 1
			0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f		//index 2
		};		

		std::shared_ptr<VertexBuffer> triangleVB;
		triangleVB.reset(VertexBuffer::Create(triangleVertices, sizeof(triangleVertices)));
		triangleVB->SetLayout({
			{ ShaderDataType::Float3, "a_position" },
			{ ShaderDataType::Float4, "a_color" }
		});
		m_triangleVertexArray->AddVertexBuffer(triangleVB);


		uint32_t triangleIndices[3] = { 0, 1, 2 };

		std::shared_ptr<IndexBuffer> triangleIB;
		triangleIB.reset(IndexBuffer::Create(triangleIndices, sizeof(triangleIndices) / sizeof(uint32_t)));
		m_triangleVertexArray->AddIndexBuffer(triangleIB);

		//square definition
		m_squareVertexArray.reset(VertexArray::Create());

		float sqrVertices[3 * 4] = {
			-0.75f, -0.75f, 0.0f,		//index 0
			0.75f, -0.75f, 0.0f, 		//index 1
			0.75f, 0.75f, 0.0f,			//index 2
			-0.75f, 0.75f, 0.0f			//index 3
		};

		std::shared_ptr<VertexBuffer> squareVB; 
		squareVB.reset(VertexBuffer::Create(sqrVertices, sizeof(sqrVertices)));
		squareVB->SetLayout({
			{ ShaderDataType::Float3, "a_position"}
		});
		m_squareVertexArray->AddVertexBuffer(squareVB);


		uint32_t sqrIndices[6] = { 0, 1, 2, 2, 3, 0 };

		std::shared_ptr<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(sqrIndices, sizeof(sqrIndices) / sizeof(uint32_t)));
		m_squareVertexArray->AddIndexBuffer(squareIB);


		std::string vertexSource = R"(
			#version 330 core

			layout(location = 0) in vec3 a_position;		
			layout(location = 1) in vec4 a_color;				

			uniform mat4 MVP;  

			out vec3 v_position;
			out vec4 v_color;

			void main()
			{
				v_position = a_position + 0.5;
				v_color = a_color;
				gl_Position = MVP * vec4(a_position, 1.0);
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

			uniform mat4 MVP;
		
			out vec3 v_position;			
			
			void main()
			{
				v_position = a_position + 0.5;				
				gl_Position = MVP * vec4(a_position, 1.0);
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


		m_triangleShader.reset(new Shader(vertexSource,fragmentSource));		
		m_squareShader.reset(new Shader(vertexSource2, fragmentSource2));

		m_camera.reset(new ProjectionCamera(640.0f, 420.0f, 0.1f, 100.0f));
	}

	Application::~Application()
	{
	}

	void Application::PushLayer(Layer * layer)
	{
		m_layerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer * overlay)
	{
		m_layerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Run()
	{		
		while (m_running)
		{
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
			RenderCommand::Clear();		

			Renderer::BeginScene(m_camera,glm::vec3(3,0,-3),glm::vec3(0,0,0));			

			Renderer::Submit(m_squareVertexArray, m_squareShader);			
			Renderer::Submit(m_triangleVertexArray, m_triangleShader);

			Renderer::EndScene();			
			
			for (Layer* layer : m_layerStack)
				layer->OnUpdate();

			m_imGuiLayer->Begin();

			for (Layer* layer : m_layerStack)
				layer->OnImGuiRender();

			m_imGuiLayer->End();

			m_window->OnUpdate();
		}
	}

	void Application::OnEvent(Event & e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		for (auto it = m_layerStack.end(); it != m_layerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.IsHandled())
				break;
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent & e)
	{
		m_running = false;
		return true;
	}
}