#include "blpch.h"
#include "Application.h"
#include "BaldLion/Rendering/Renderer.h"
#include "BaldLion/Core/Threading/TaskScheduler.h"
#include "BaldLion/Animation/AnimationManager.h"
#include "BaldLion/SceneManagement/SceneManager.h"
#include "BaldLion/ResourceManagement/ResourceManager.h"
#include "BaldLion/Physics/PhysicsManager.h"
#include "BaldLion/AI/Navigation/NavigationManager.h"
#include "BaldLion/AI/HTN/HTNManager.h"
#include "BaldLion/Core/Input.h"
#include "BaldLion/Core/EventManager.h"

#include <functional>
#include <GLFW/glfw3.h>


namespace BaldLion
{
	Application* Application::s_instance = nullptr;

	Application::Application(const std::string& applicationName)
	{
		BL_PROFILE_FRAME();

		BL_CORE_ASSERT(!s_instance, "Application already exists");

		s_instance = this;

		Memory::MemoryManager::Init(0);
		Threading::TaskScheduler::Init();
		EventManager::Init();
		ResourceManagement::ResourceManager::Init();		

		m_window = Window::Create(WindowProps(applicationName));
		EventManager::RegisterEventHandler("WindowClosedEvent", BL_BIND_OBJECT_FUNCTION(Application::OnWindowClose));
		EventManager::RegisterEventHandler("WindowMinimizedEvent", BL_BIND_OBJECT_FUNCTION(Application::OnWindowMinimized));

		Time::Init();

		AI::Navigation::NavigationManager::Init();
		AI::HTN::HTNManager::Init();

		Animation::AnimationManager::Init();
		SceneManagement::SceneManager::Init();
		Rendering::Renderer::Init(m_window->GetWidth(), m_window->GetHeight());
		Physics::PhysicsManager::Init(1.0f/60.0f);
		Input::InputSystem::Init();

		ResourceManagement::ResourceManager::LoadAssets();

		m_layerStack.Init();

		m_imGuiLayer = MemoryManager::New<ImGuiLayer>("ImGuiLayer",Memory::AllocationType::FreeList_Main);
		PushOverlay(m_imGuiLayer);
	}

	Application::~Application()
	{		
		OPTICK_SHUTDOWN();

		m_layerStack.PopOverlay(m_imGuiLayer);
		m_layerStack.Delete();

		Input::InputSystem::Stop();
		Physics::PhysicsManager::Stop();
		Rendering::Renderer::Stop();
		SceneManagement::SceneManager::Stop();
		Animation::AnimationManager::Stop();

		AI::HTN::HTNManager::Stop();
		AI::Navigation::NavigationManager::Stop();

		Time::Stop();

		EventManager::UnregisterEventHandler("WindowClosedEvent", BL_BIND_OBJECT_FUNCTION(Application::OnWindowClose));
		EventManager::UnregisterEventHandler("WindowMinimizedEvent", BL_BIND_OBJECT_FUNCTION(Application::OnWindowMinimized));

		Window::Destroy(m_window);

		ResourceManagement::ResourceManager::Stop();
		EventManager::Stop();
		Threading::TaskScheduler::Stop();
		Memory::MemoryManager::Stop();
	}

	void Application::PushLayer(GameStateLayer * layer)
	{
		BL_PROFILE_FUNCTION();

		m_layerStack.PushLayer(layer);		
	}

	void Application::PushOverlay(GameStateLayer * overlay)
	{
		BL_PROFILE_FUNCTION();

		m_layerStack.PushOverlay(overlay);		
	}

	void Application::PopLayer(GameStateLayer* layer)
	{
		BL_PROFILE_FUNCTION();
		m_layerStack.PopLayer(layer);
	}

	void Application::PopOverlay(GameStateLayer* overlay)
	{
		BL_PROFILE_FUNCTION();
		m_layerStack.PopOverlay(overlay);
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
			bool processNewFrame = false;

			Time::UpdateCurrentTime(glfwGetTime(), processNewFrame);

			if (!processNewFrame)// Platform::GetTime
			{
				continue;
			}
	
			BL_PROFILE_FRAME();

			Input::InputSystem::UpdateEntries();			

			if (!m_minimized)
			{
				{
					BL_PROFILE_SCOPE("LayerStack OnUpdates", Optick::Category::GameLogic);
					m_layerStack[m_layerStack.Size() - 1]->OnUpdate();
				}

				m_imGuiLayer->Begin();

				{
					BL_PROFILE_SCOPE("LayerStack OnImGuiRender", Optick::Category::Type::GameLogic);
					for (ui32 i = 0; i < m_layerStack.Size(); ++i)
						m_layerStack[i]->OnImGuiRender();
				}

				m_imGuiLayer->End();
			}

			m_window->OnUpdate();		

			EventManager::Update();

			MemoryManager::Delete(AllocationType::Linear_Frame);
		}	
	}

	bool Application::OnWindowClose(const EventEntry& e)
	{
		Close();
		return true;
	}

	bool Application::OnWindowMinimized(const EventEntry& e)
	{
		m_minimized = (bool)e.eventData1;
		return true;
	}

}