#include "blpch.h"
#include "WindowsWindow.h"

#include "BaldLion/Core/EventManager.h"
#include "BaldLion/Rendering/Platform/OpenGL/OpenGLContext.h"
#include "BaldLion/Core/Input.h"

#include <glad/glad.h>

namespace BaldLion
{
	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		BL_LOG_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	WindowsWindow::WindowsWindow(const WindowProps & props)
	{
		BL_PROFILE_FUNCTION();
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps & props)
	{
		BL_PROFILE_FUNCTION();

		m_data.Title = props.Title;
		m_data.Width = props.Width;
		m_data.Height = props.Height;		

		BL_LOG_CORE_INFO("Creating window {0} ({1}, {2})", BL_STRINGID_TO_STR_C(props.Title), props.Width, props.Height);

		if (!s_GLFWInitialized)
		{
			int success = glfwInit();			
			BL_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}
		
		glfwWindowHint(GLFW_SAMPLES, 4);
		m_window = glfwCreateWindow((int)props.Width, (int)props.Height, BL_STRINGID_TO_STR_C(props.Title), nullptr, nullptr);
		m_context = new Rendering::OpenGLContext(m_window);

		m_context->Init();			   		 

		glfwSetWindowUserPointer(m_window, &m_data);
		SetVSync(true);

		//Set GLFW Callbacks
		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			bool wasMinimized = data.Width == 0 || data.Height == 0;

			data.Width = width;
			data.Height = height;

			EventEntry e;
			e.senderID = BL_STRING_TO_STRINGID("WindowsPlatformWindow");

			if (wasMinimized || width == 0 || height == 0)
			{
				e.eventID = BL_STRING_TO_STRINGID("WindowMinimizedEvent");
				e.eventData1 = (width == 0 || height == 0);				
			}
			else
			{
				e.eventID = BL_STRING_TO_STRINGID("WindowResizedEvent");
				e.eventData1 = width;
				e.eventData2 = height;
			}

			EventManager::QueueEvent(e);
		});		

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			EventEntry e;
			e.senderID = BL_STRING_TO_STRINGID("WindowsPlatformWindow");
			e.eventID = BL_STRING_TO_STRINGID("WindowClosedEvent");

			EventManager::QueueEvent(e);
		});

		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			EventEntry e;
			e.senderID = BL_STRING_TO_STRINGID("WindowsPlatformWindow");			
			e.eventData1 = key;

			switch (action)
			{
				case GLFW_PRESS:		
				{
					e.eventID = BL_STRING_TO_STRINGID("KeyPressedEvent");
					e.eventData2 = 0;
					
					EventManager::QueueEvent(e);
					break;
				}
				case GLFW_RELEASE:
				{
					e.eventID = BL_STRING_TO_STRINGID("KeyReleasedEvent");
					
					EventManager::QueueEvent(e);
					break;
				}
				case GLFW_REPEAT:
				{
					e.eventID = BL_STRING_TO_STRINGID("KeyPressedEvent");
					e.eventData2 = 1;
					EventManager::QueueEvent(e);
					break;
				}
			}
		});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int modes)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			EventEntry e;
			e.senderID = BL_STRING_TO_STRINGID("WindowsPlatformWindow");

			switch (action)
			{
				case GLFW_PRESS:
				{
					e.eventID = BL_STRING_TO_STRINGID("MouseButtonPressedEvent");
					EventManager::QueueEvent(e);
					break;
				}
				case GLFW_RELEASE:
				{
					e.eventID = BL_STRING_TO_STRINGID("MouseButtonReleasedEvent");
					EventManager::QueueEvent(e);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			EventEntry e;
			e.senderID = BL_STRING_TO_STRINGID("WindowsPlatformWindow");
			e.eventID = BL_STRING_TO_STRINGID("MouseScrolledEvent");
			e.eventData1 = (float)xOffset;
			e.eventData2 = (float)yOffset;

			EventManager::QueueEvent(e);
		});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			EventEntry e;
			e.senderID = BL_STRING_TO_STRINGID("WindowsPlatformWindow");
			e.eventID = BL_STRING_TO_STRINGID("MouseMovedEvent");
			e.eventData1 = (float)xPos;
			e.eventData2 = (float)yPos;

			EventManager::QueueEvent(e);
		});

		glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			EventEntry e;
			e.senderID = BL_STRING_TO_STRINGID("WindowsPlatformWindow");
			e.eventID = BL_STRING_TO_STRINGID("KeyTypedEvent");
			e.eventData1 = keycode;

			EventManager::QueueEvent(e);
		});

		glfwSetJoystickCallback([](int joystickId, int event)
			{
				if (event == GLFW_CONNECTED)
				{
					Input::InputSystem::SetGamepadIsConnected(true);
					BL_LOG_CORE_INFO("joystick {0} connected", joystickId);

				}
				else if (event == GLFW_DISCONNECTED)
				{
					Input::InputSystem::SetGamepadIsConnected(false);
					BL_LOG_CORE_INFO("joystick {0} disconnected", joystickId);
				}

			});
	}

	void WindowsWindow::Shutdown()
	{
		BL_PROFILE_FUNCTION();

		glfwDestroyWindow(m_window);
	}

	void WindowsWindow::OnUpdate()
	{
		BL_PROFILE_FUNCTION();

		glfwPollEvents();
		m_context->SwapBuffers();		
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_data.VSync;
	}
}