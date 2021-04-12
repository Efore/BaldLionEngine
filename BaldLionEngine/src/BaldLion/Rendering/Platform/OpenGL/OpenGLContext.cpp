#include "blpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace BaldLion
{
	namespace Rendering
	{
		OpenGLContext::OpenGLContext(GLFWwindow * windowHandle) : m_windowHandle(windowHandle)
		{
			BL_CORE_ASSERT(m_windowHandle, "Window handle is null!");
		}

		void OpenGLContext::Init()
		{
			OPTICK_EVENT();

			glfwMakeContextCurrent(m_windowHandle);
			int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
			BL_CORE_ASSERT(status, "Failed to initialize Glad");

			BL_LOG_CORE_INFO("OpenGL Info:");
			BL_LOG_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
			BL_LOG_CORE_INFO("  REnderer: {0}", glGetString(GL_RENDERER));
			BL_LOG_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));
		}

		void OpenGLContext::SwapBuffers()
		{
			OPTICK_EVENT();

			BL_CORE_ASSERT(m_windowHandle, "Window handle is null!");
			glfwSwapBuffers(m_windowHandle);
		}
	}
}