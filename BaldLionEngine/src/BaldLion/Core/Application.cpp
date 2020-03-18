#include "blpch.h"
#include "Application.h"
#include "BaldLion/Rendering/Renderer.h"

#include <GLFW/glfw3.h>

namespace BaldLion
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_instance = nullptr;

	Application::Application()
	{
		BL_PROFILE_FUNCTION();

		BL_CORE_ASSERT(!s_instance, "Application already exists");

		s_instance = this;

		m_window = Window::Create();
		m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		Rendering::Renderer::Init();

		m_lastFrameTime = 0.0f;

		m_imGuiLayer = new ImGuiLayer();
		PushOverlay(m_imGuiLayer);

	}

	Application::~Application()
	{
	}

	void Application::PushLayer(Layer * layer)
	{
		BL_PROFILE_FUNCTION();

		m_layerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer * overlay)
	{
		BL_PROFILE_FUNCTION();

		m_layerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Run()
	{		
		BL_PROFILE_FUNCTION();

		while (m_running)
		{	
			BL_PROFILE_SCOPE("RunLoop");

			float time = (float)glfwGetTime(); // Platform::GetTime

			TimeStep timeStep = time - m_lastFrameTime;

			m_lastFrameTime = time;

			if (!m_minimized)
			{
				{
					BL_PROFILE_SCOPE("LayerStack OnUpdates");

					for (Layer* layer : m_layerStack)
						layer->OnUpdate(timeStep);
				}

				m_imGuiLayer->Begin();

				{
					BL_PROFILE_SCOPE("LayerStack OnImGuiRender");
					for (Layer* layer : m_layerStack)
						layer->OnImGuiRender();
				}

				m_imGuiLayer->End();
			}


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