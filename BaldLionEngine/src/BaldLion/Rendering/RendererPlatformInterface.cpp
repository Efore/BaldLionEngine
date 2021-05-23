#include "blpch.h"
#include "RendererPlatformInterface.h"

#include "Platform/OpenGL/OpenGLRenderer.h"

namespace BaldLion
{
	namespace Rendering
	{
		RendererPlatformInterface::RendererPlatform RendererPlatformInterface::s_rendererPlatform = RendererPlatformInterface::RendererPlatform::OpenGL;

		RendererPlatformInterface* RendererPlatformInterface::Create()
		{
			switch (s_rendererPlatform)
			{
			case RendererPlatformInterface::RendererPlatform::None:			BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererPlatformInterface::RendererPlatform::OpenGL:		return MemoryManager::New<OpenGLRenderer>("OpenGLRenderer", AllocationType::FreeList_Renderer);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}
	}
}