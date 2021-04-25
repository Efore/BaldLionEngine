/*****************************************************************//**
 * \file   VertexArray.cpp
 * \brief  
 * 
 * \author Iván León Santiago
 * \date   April 2021
 *********************************************************************/

#include "blpch.h"
#include "VertexArray.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

#include "Renderer.h"
#include "RendererPlatformInterface.h"

namespace BaldLion
{
	namespace Rendering
	{
		VertexArray* VertexArray::Create()
		{
			switch (RendererPlatformInterface::GetAPI())
			{
			case RendererPlatformInterface::RendererPlatform::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererPlatformInterface::RendererPlatform::OpenGL:	return MemoryManager::New<OpenGLVertexArray>("Vertex array", AllocationType::FreeList_Renderer);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}

		void VertexArray::Destroy(VertexArray* vertexArray)
		{
			MemoryManager::Delete(vertexArray);
		}

	}
}