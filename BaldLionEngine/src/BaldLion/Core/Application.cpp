#include "blpch.h"
#include "Application.h"
#include "BaldLion/Rendering/Renderer.h"
#include "BaldLion/Core/Memory/MemoryManager.h"
#include <GLFW/glfw3.h>


namespace BaldLion
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_instance = nullptr;

	Application::Application(const std::string& applicationName)
	{
		BL_PROFILE_FUNCTION();

		BL_CORE_ASSERT(!s_instance, "Application already exists");

		s_instance = this;
		Memory::MemoryManager::Init(0);

		m_window = Window::Create(WindowProps(applicationName));
		m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		Rendering::Renderer::Init();

		m_lastFrameTime = 0.0f;

		m_imGuiLayer = new ImGuiLayer();
		PushOverlay(m_imGuiLayer);

	}

	Application::~Application()
	{
		Memory::MemoryManager::Clear();
	}

	void Application::PushLayer(Layer * layer)
	{
		BL_PROFILE_FUNCTION();

		m_layerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer * overlay)
	{
		BL_PROFILE_FUNCTION();

		m_layerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Close()
	{		
		Rendering::Renderer::Stop();
		m_running = false;
	}

	Application& Application::GetInstance(const std::string& applicationName)
	{
		if (s_instance == nullptr)
			s_instance = new Application(applicationName);

		return *s_instance;
	}

	void Application::Run()
	{		
		BL_PROFILE_FUNCTION();

		while (m_running)
		{	
			BL_PROFILE_SCOPE("RunLoop");

			float time = (float)glfwGetTime(); // Platform::GetTime

			TimeStep timeStep = time - m_lastFrameTime;

			m_lastFrameTime = time;

			if (!m_minimized)
			{
				{
					BL_PROFILE_SCOPE("LayerStack OnUpdates");

					for (Layer* layer : m_layerStack)
						layer->OnUpdate(timeStep);
				}

				m_imGuiLayer->Begin();

				{
					BL_PROFILE_SCOPE("LayerStack OnImGuiRender");
					for (Layer* layer : m_layerStack)
						layer->OnImGuiRender();
				}

				m_imGuiLayer->End();
			}

			m_window->OnUpdate();

			Memory::MemoryManager::Clear(Memory::AllocationType::Linear_Frame);
		}
		
		for (Layer* layer : m_layerStack)
			layer->OnDetach();		
	}

	void Application::OnEvent(Event & e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowMinimizeEvent>(BIND_EVENT_FN(OnWindowMinimized));

		for (auto it = m_layerStack.end(); it != m_layerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.IsHandled())
				break;
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent & e)
	{
		Close();
		return true;
	}

	bool Application::OnWindowMinimized(WindowMinimizeEvent& e)
	{
		m_minimized = e.GetMinimized();
		return true;
	}

}