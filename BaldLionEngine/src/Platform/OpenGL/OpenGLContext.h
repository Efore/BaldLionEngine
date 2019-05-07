#pragma once
#include "BaldLion/Renderer/GraphicsContext.h"


struct GLFWwindow;

namespace BaldLion
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_windowHandle;
	};

}