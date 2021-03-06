#pragma once

#include <glm/glm.hpp>
#include "VertexArray.h"

namespace BaldLion
{
	namespace Rendering
	{
		class RendererPlatformInterface
		{
		public:

			enum class RendererPlatform
			{
				None = 0, OpenGL = 1
			};

			virtual void Init() = 0;
			virtual void SetViewport(ui32 x, ui32 y, ui32 width, ui32 height) = 0;

			virtual void SetClearColor(const glm::vec4& color) = 0;
			virtual void Clear() = 0;

			virtual void DrawIndexed(const VertexArray* vertexArray) = 0;

			inline static RendererPlatform GetAPI() { return s_rendererPlatform; }

		private:
			static RendererPlatform s_rendererPlatform;
		};
	}
}