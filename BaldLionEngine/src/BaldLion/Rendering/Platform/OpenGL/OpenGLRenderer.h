#pragma once

#include "BaldLion/Rendering/RendererPlatformInterface.h"

namespace BaldLion
{
	namespace Rendering
	{
		class OpenGLRenderer : public RendererPlatformInterface
		{
		public:
			virtual void Init() override;
			virtual void SetViewport(ui32 x, ui32 y, ui32 width, ui32 height) override;

			virtual void SetClearColor(const glm::vec4& color) override;
			virtual void Clear() override;

			virtual void DrawVertexArray(const VertexArray* vertexArray) override;
			virtual void SetFaceCulling(FaceCulling faceCulling) const override;
		};
	}
}
