#include "blpch.h"
#include "Application.h"

#include "BaldLion/Log.h"

#include <glad/glad.h>


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
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);

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