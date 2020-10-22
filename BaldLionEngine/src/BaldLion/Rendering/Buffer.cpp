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
		VertexBuffer* VertexBuffer::Create(const void * vertices, uint32_t size)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
			case RendererPlatformInterface::RendererPlatform::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererPlatformInterface::RendererPlatform::OpenGL:	return MemoryManager::New<OpenGLVertexBuffer>("vertex buffer", AllocationType::FreeList_Renderer, vertices, size);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}

		//-------------------------------------------------
		//Index buffer
		//-------------------------------------------------
		IndexBuffer* IndexBuffer::Create(const uint32_t * indices, uint32_t count)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
				case RendererPlatformInterface::RendererPlatform::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
				case RendererPlatformInterface::RendererPlatform::OpenGL:	return  MemoryManager::New<OpenGLIndexBuffer>("index buffer", AllocationType::FreeList_Renderer, indices, count);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}
	}

}