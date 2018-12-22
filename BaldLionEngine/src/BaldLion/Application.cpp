#include "blpch.h"

#include "Application.h"

#include "BaldLion/Events/ApplicationEvent.h"
#include "BaldLion/Log.h"

#include <GLFW/glfw3.h>

namespace BaldLion
{
	Application::Application()
	{
		m_window = std::unique_ptr<Window>(Window::Create());
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
}