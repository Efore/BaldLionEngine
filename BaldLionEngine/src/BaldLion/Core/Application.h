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
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() const { return *m_window; }

		virtual void Close();

		static Application& GetInstance(const std::string& applicationName = "");

	protected:
		Application(const std::string& applicationName);

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowMinimized(WindowMinimizeEvent& e);

	protected:
		Window* m_window;
		ImGuiLayer* m_imGuiLayer;

		bool m_running = true;
		bool m_minimized = false;

		LayerStack m_layerStack;
		float m_lastFrameTime;		

		static Application* s_instance;
	};

	//To be defined in client
	Application& CreateApplication(const std::string& applicationName = "");
}

