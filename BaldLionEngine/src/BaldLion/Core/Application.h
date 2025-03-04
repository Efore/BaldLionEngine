#pragma once

#include "Core.h" 
#include "Window.h"

#include "BaldLion/Core/EventManager.h"
#include "BaldLion/Core/GameStateLayerStack.h"

#include "BaldLion/ImGui/ImGuiLayer.h"

namespace BaldLion 
{
	class Application
	{
	public:	
		virtual ~Application();

		void Run();

		void PushLayer(GameStateLayer* layer);
		void PushOverlay(GameStateLayer* overlay);

		void PopLayer(GameStateLayer* layer);
		void PopOverlay(GameStateLayer* overlay);

		inline Window& GetWindow() const { return *m_window; }

		virtual void Close();

		static Application& GetInstance(const std::string& applicationName = "");

	protected:
		Application(const std::string& applicationName);

		bool OnWindowClose(const EventEntry& e);
		bool OnWindowMinimized(const EventEntry& e);

	protected:
		Window* m_window;
		ImGuiLayer* m_imGuiLayer;

		bool m_running = true;
		bool m_minimized = false;

		GameStateLayerStack m_layerStack;

		static Application* s_instance;
	};

	//To be defined in client
	Application& CreateApplication(const std::string& applicationName = "");
}

