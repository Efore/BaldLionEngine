#pragma once
#include "BaldLion/Rendering/GraphicsContext.h"


struct GLFWwindow;
 
namespace BaldLion
{
	namespace Rendering
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
}