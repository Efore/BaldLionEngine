#include "blpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "RendererPlatformInterface.h"
#include "Platform/OpenGL/OpenGLBuffer.h"


namespace BaldLion
{
	namespace Rendering
	{
		//-------------------------------------------------
		//Vertex buffer
		//-------------------------------------------------
		Ref<VertexBuffer> VertexBuffer::Create(float * vertices, uint32_t size)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
			case RendererPlatformInterface::RendererPlatform::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererPlatformInterface::RendererPlatform::OpenGL:		return CreateRef<OpenGLVertexBuffer>(vertices, size);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}

		Ref<VertexBuffer> VertexBuffer::Create(Vertex* vertices, uint32_t size)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
			case RendererPlatformInterface::RendererPlatform::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererPlatformInterface::RendererPlatform::OpenGL:		return CreateRef<OpenGLVertexBuffer>(vertices[0].GetFirstElement(), size);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}

		//-------------------------------------------------
		//Index buffer
		//-------------------------------------------------
		Ref<IndexBuffer> IndexBuffer::Create(uint32_t * indices, uint32_t count)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
				case RendererPlatformInterface::RendererPlatform::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
				case RendererPlatformInterface::RendererPlatform::OpenGL:		return CreateRef<OpenGLIndexBuffer>(indices, count);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}
	}

}