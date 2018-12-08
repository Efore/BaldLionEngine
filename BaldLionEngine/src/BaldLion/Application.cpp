#include "blpch.h"

#include "Application.h"

#include "BaldLion/Events/ApplicationEvent.h"
#include "BaldLion/Log.h"

namespace BaldLion
{
	Application::Application()
	{
	}


	Application::~Application()
	{
	}

	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
	
		if (e.IsInCategory(EventCategoryApplication))
		{
			BL_LOG_TRACE(e);
		}
		if (e.IsInCategory(EventCategoryInput))
		{
			BL_LOG_TRACE(e);
		}

		while (true);
	}
}