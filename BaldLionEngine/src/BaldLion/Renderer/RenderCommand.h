#pragma once
#include "Renderer.h"

namespace BaldLion
{
	class RenderCommand
	{
	public:
		inline static void SetClearColor(const glm::vec4& color)
		{
			s_renderer->SetClearColor(color);
		}

		inline static void Clear()
		{
			s_renderer->Clear();
		}

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray)
		{
			s_renderer->DrawIndexed(vertexArray);
		} 
	private :
		static RendererAPI *s_renderer;
	};
}