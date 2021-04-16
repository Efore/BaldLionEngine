#include "blpch.h"
#include "OpenGLRenderer.h"

#include <glad/glad.h>

namespace BaldLion
{
	namespace Rendering
	{
		void OpenGLRenderer::Init()
		{
			BL_PROFILE_FUNCTION();

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glEnable(GL_MULTISAMPLE);
			glDepthMask(GL_TRUE);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
		}

		void OpenGLRenderer::SetViewport(ui32 x, ui32 y, ui32 width, ui32 height)
		{
			BL_PROFILE_FUNCTION();

			glViewport(x, y, width, height);
		}

		void OpenGLRenderer::SetClearColor(const glm::vec4& color)
		{
			BL_PROFILE_FUNCTION();

			glDepthMask(GL_TRUE);
			glClearDepth(1.0f);
			glClearColor(color.r, color.g, color.b, color.a);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void OpenGLRenderer::Clear()
		{		
			BL_PROFILE_FUNCTION();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void OpenGLRenderer::DrawIndexed(const VertexArray* vertexArray)
		{
			BL_PROFILE_FUNCTION();

			glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
		}
	}
}