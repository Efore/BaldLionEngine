#include "blpch.h"
#include "WindowsWindow.h"

#include "BaldLion/Events/MouseEvent.h"
#include "BaldLion/Events/ApplicationEvent.h"
#include "BaldLion/Events/KeyEvent.h"
#include "BaldLion/Rendering/Platform/OpenGL/OpenGLContext.h"

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

		BL_LOG_CORE_INFO("Creating window {0} ({1}, {2})", ID_TO_STRING(props.Title), props.Width, props.Height);

		if (!s_GLFWInitialized)
		{
			int success = glfwInit();			
			BL_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}
		
		glfwWindowHint(GLFW_SAMPLES, 4);
		m_window = glfwCreateWindow((int)props.Width, (int)props.Height, ID_TO_STRING(props.Title), nullptr, nullptr);
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

			if (wasMinimized || width == 0 || height == 0)
			{
				WindowMinimizeEvent event(width == 0 || height == 0);
				data.EventCallback(event);
			}
			else
			{
				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			}
		});		

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:		
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int modes)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});

		glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.EventCallback(event);
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