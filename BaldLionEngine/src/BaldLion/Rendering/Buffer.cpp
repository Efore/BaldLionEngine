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
		VertexBuffer* VertexBuffer::Create(const void * vertices, ui32 size)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
			case RendererPlatformInterface::RendererPlatform::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererPlatformInterface::RendererPlatform::OpenGL:	return MemoryManager::New<OpenGLVertexBuffer>("Vertex buffer", AllocationType::FreeList_Renderer, vertices, size);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}

		//-------------------------------------------------
		//Index buffer
		//-------------------------------------------------
		IndexBuffer* IndexBuffer::Create(const ui32 * indices, ui32 count)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
				case RendererPlatformInterface::RendererPlatform::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
				case RendererPlatformInterface::RendererPlatform::OpenGL:	return  MemoryManager::New<OpenGLIndexBuffer>("Index buffer", AllocationType::FreeList_Renderer, indices, count);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}
	}

}