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

			enum class FaceCulling {
				Front,
				Back,
				Both
			};

			virtual void Init() = 0;
			virtual void SetViewport(ui32 x, ui32 y, ui32 width, ui32 height) = 0;

			virtual void SetClearColor(const glm::vec4& color) = 0;
			virtual void Delete() = 0;

			virtual void SetFaceCulling(FaceCulling faceCullint) const = 0;

			virtual void DrawVertexArray(const VertexArray* vertexArray) = 0;

			static RendererPlatformInterface* Create();

			inline static RendererPlatform GetAPI() { return s_rendererPlatform; }

		private:
			static RendererPlatform s_rendererPlatform;
		};
	}
}