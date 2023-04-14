#include "blpch.h"
#include "Application.h"
#include "BaldLion/Rendering/Renderer.h"
#include "BaldLion/Core/JobManagement/JobManager.h"
#include "BaldLion/Animation/AnimationManager.h"
#include "BaldLion/SceneManagement/SceneManager.h"
#include "BaldLion/ResourceManagement/ResourceManager.h"
#include "BaldLion/Physics/PhysicsManager.h"
#include "BaldLion/AI/Navigation/NavigationManager.h"

#include <GLFW/glfw3.h>


namespace BaldLion
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_instance = nullptr;

	Application::Application(const std::string& applicationName)
	{
		BL_PROFILE_FRAME();

		BL_CORE_ASSERT(!s_instance, "Application already exists");

		s_instance = this;

		Memory::MemoryManager::Init(0);
		JobManagement::JobManager::Init();
		ResourceManagement::ResourceManager::Init();		
		AI::Navigation::NavigationManager::Init();

		m_window = Window::Create(WindowProps(applicationName));
		m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		Time::Init();
		Animation::AnimationManager::Init();
		SceneManagement::SceneManager::Init();
		Rendering::Renderer::Init(m_window->GetWidth(), m_window->GetHeight());
		ResourceManagement::ResourceManager::LoadAssets();
		Physics::PhysicsManager::Init(1.0f/60.0f);

		m_layerStack.Init();

		m_imGuiLayer = MemoryManager::New<ImGuiLayer>("ImGuiLayer",Memory::AllocationType::FreeList_Main);
		PushOverlay(m_imGuiLayer);
	}

	Application::~Application()
	{
		JobManagement::JobManager::WaitForJobs();

		m_layerStack.PopOverlay(m_imGuiLayer);
		m_layerStack.Delete();
		Window::Destroy(m_window);

		Rendering::Renderer::Stop();
		SceneManagement::SceneManager::Stop	();
		Animation::AnimationManager::Stop();
		Physics::PhysicsManager::Stop();

		Time::Stop();

		AI::Navigation::NavigationManager::Stop();
		ResourceManagement::ResourceManager::Stop();
		JobManagement::JobManager::Stop();
		Memory::MemoryManager::Stop();

		OPTICK_SHUTDOWN();
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

			if (!Time::SetCurrentTime(glfwGetTime()))// Platform::GetTime
			{
				continue;
			}
	
			if (!m_minimized)
			{
				{
					BL_PROFILE_SCOPE("LayerStack OnUpdates", Optick::Category::GameLogic);

					for (ui32 i = 0; i < m_layerStack.Size(); ++i)
						m_layerStack[i]->OnUpdate();
				}

				m_imGuiLayer->Begin();

				{
					BL_PROFILE_SCOPE("LayerStack OnUpdates", Optick::Category::Type::GameLogic);
					for (ui32 i = 0; i < m_layerStack.Size(); ++i)
						m_layerStack[i]->OnImGuiRender();
				}

				m_imGuiLayer->End();
			}

			m_window->OnUpdate();			
			MemoryManager::Delete(AllocationType::Linear_Frame);
		}	
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