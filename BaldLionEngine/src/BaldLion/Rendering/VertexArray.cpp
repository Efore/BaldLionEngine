#include "blpch.h"
#include "VertexArray.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

#include "Renderer.h"
#include "RendererAPI.h"

namespace BaldLion
{
	namespace Rendering
	{
		Ref<VertexArray> VertexArray::Create()
		{
			switch (RendererAPI::GetAPI())
			{
			case RendererAPI::API::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererAPI::API::OpenGL:		return CreateRef<OpenGLVertexArray>();
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}
	}
}