#pragma once

#include "Core.h" 
#include "Window.h"

#include "BaldLion/Events/ApplicationEvent.h"
#include "BaldLion/LayerStack.h"

namespace BaldLion 
{
	class BL_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() const { return *m_window; }

		inline static Application& Get(){ return *s_instance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_window;
		bool m_running = true;
		LayerStack m_layerStack;

		static Application* s_instance;
	};

	//To be defined in client
	Application* CreateApplication();
}

