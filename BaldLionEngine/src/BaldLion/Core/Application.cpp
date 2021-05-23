#include "blpch.h"
#include "Application.h"
#include "BaldLion/Rendering/Renderer.h"
#include "BaldLion/Core/Memory/MemoryManager.h"
#include "BaldLion/Core/JobManagement/JobManager.h"
#include <GLFW/glfw3.h>


namespace BaldLion
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_instance = nullptr;

	Application::Application(const std::string& applicationName)
	{
		BL_PROFILE_FUNCTION();

		BL_CORE_ASSERT(!s_instance, "Application already exists");

		s_instance = this;

		Memory::MemoryManager::Init(0);
		JobManagement::JobManager::Init();

		m_window = Window::Create(WindowProps(applicationName));
		m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		Rendering::Renderer::Init(m_window->GetWidth(), m_window->GetHeight());

		m_layerStack.Init();

		m_lastFrameTime = 0.0f;

		m_imGuiLayer = MemoryManager::New<ImGuiLayer>("ImGuiLayer",Memory::AllocationType::FreeList_Main);
		PushOverlay(m_imGuiLayer);
	}

	Application::~Application()
	{
		m_layerStack.PopOverlay(m_imGuiLayer);
		m_layerStack.Clear();
		Window::Destroy(m_window);
		Rendering::Renderer::Stop();
		JobManagement::JobManager::Stop();
		Memory::MemoryManager::Stop();
	}

	void Application::PushLayer(Layer * layer)
	{
		BL_PROFILE_FUNCTION();

		m_layerStack.PushLayer(layer);		
	}

	void Application::PushOverlay(Layer * overlay)
	{
		BL_PROFILE_FUNCTION();

		m_layerStack.PushOverlay(overlay);		
	}

	void Application::Close()
	{	
		m_running = false;
	}

	Application& Application::GetInstance(const std::string& applicationName)
	{
		if (s_instance == nullptr)
			s_instance = new Application(applicationName);

		return *s_instance;
	}

	void Application::Run()
	{		

		while (m_running)
		{	
			BL_PROFILE_FRAME();

			float time = (float)glfwGetTime(); // Platform::GetTime

			TimeStep timeStep = time - m_lastFrameTime;

			m_lastFrameTime = time;

			if (!m_minimized)
			{
				{
					BL_PROFILE_SCOPE("LayerStack OnUpdates", Optick::Category::GameLogic);

					for (ui32 i = 0; i < m_layerStack.Size(); ++i)
						m_layerStack[i]->OnUpdate(timeStep);
				}

				m_imGuiLayer->Begin();

				{
					BL_PROFILE_SCOPE("LayerStack OnUpdates", Optick::Category::Type::GameLogic);
					for (ui32 i = 0; i < m_layerStack.Size(); ++i)
						m_layerStack[i]->OnImGuiRender(timeStep);
				}

				m_imGuiLayer->End();
			}

			m_window->OnUpdate();			
			MemoryManager::Clear(AllocationType::Linear_Frame);
		}	

		OPTICK_SHUTDOWN();
	}

	void Application::OnEvent(Event & e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowMinimizeEvent>(BIND_EVENT_FN(OnWindowMinimized));

		for (ui32 i = 0; i < m_layerStack.Size(); ++i)
		{
			m_layerStack[i]->OnEvent(e);
			if (e.IsHandled())
				break;
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent & e)
	{
		Close();
		return true;
	}

	bool Application::OnWindowMinimized(WindowMinimizeEvent& e)
	{
		m_minimized = e.GetMinimized();
		return true;
	}

}