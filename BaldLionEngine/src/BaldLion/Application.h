#pragma once

#include "Core.h" 
#include "Window.h"

#include "BaldLion/Events/ApplicationEvent.h"
#include "BaldLion/LayerStack.h"

#include "BaldLion/ImGui/ImGuiLayer.h"
#include "BaldLion/Renderer/Shader.h"
#include "BaldLion/Renderer/Buffer.h"
#include "BaldLion/Renderer/VertexArray.h"
#include "BaldLion/Renderer/Camera.h"

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
		ImGuiLayer* m_imGuiLayer;

		bool m_running = true;
		LayerStack m_layerStack;

		static Application* s_instance;

		std::shared_ptr<Shader> m_triangleShader;
		std::shared_ptr<Shader> m_squareShader;

		std::shared_ptr<VertexArray> m_triangleVertexArray;
		std::shared_ptr<VertexArray> m_squareVertexArray;

		std::shared_ptr<ProjectionCamera> m_camera;
	};

	//To be defined in client
	Application* CreateApplication();
}

