#pragma once

#include "Core.h" 
#include "Window.h"

#include "BaldLion/Events/ApplicationEvent.h"
#include "BaldLion/Core/LayerStack.h"

#include "BaldLion/Core/TimeStep.h"
#include "BaldLion/ImGui/ImGuiLayer.h"

namespace BaldLion 
{
	class Application
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

	private:
		std::unique_ptr<Window> m_window;
		ImGuiLayer* m_imGuiLayer;

		bool m_running = true;
		bool m_minimized = false;

		LayerStack m_layerStack;
		float m_lastFrameTime;
		

	private:
		static Application* s_instance;
	};

	//To be defined in client
	Application* CreateApplication();
}

