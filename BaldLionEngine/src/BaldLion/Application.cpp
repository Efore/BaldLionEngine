#include "blpch.h"
#include "Application.h"

#include "BaldLion/Log.h"

#include <GLFW/glfw3.h>

namespace BaldLion
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)


	Application::Application()
	{
		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{		
		while (m_running)
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_window->OnUpdate();
		}
	}
	void Application::OnEvent(Event & e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		BL_LOG_CORE_TRACE("{0}", e);
	}

	bool Application::OnWindowClose(WindowCloseEvent & e)
	{
		m_running = false;
		return true;
	}
}