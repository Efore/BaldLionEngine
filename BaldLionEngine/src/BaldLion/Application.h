#pragma once

#include "Core.h" 

namespace BaldLion 
{
	class BL_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	//To be defined in client
	Application* CreateApplication();
}

