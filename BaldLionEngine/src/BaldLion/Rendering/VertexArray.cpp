#include "blpch.h"
#include "VertexArray.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

#include "Renderer.h"
#include "RendererPlatformInterface.h"

namespace BaldLion
{
	namespace Rendering
	{
		Ref<VertexArray> VertexArray::Create()
		{
			switch (RendererPlatformInterface::GetAPI())
			{
			case RendererPlatformInterface::RendererPlatform::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererPlatformInterface::RendererPlatform::OpenGL:		return CreateRef<OpenGLVertexArray>();
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}
	}
}