#pragma once

#include "RendererPlatformInterface.h"
#include "BaldLion/Rendering/Platform/OpenGL/OpenGLDebugRenderInterface.h"
#include "BaldLion/Core/Memory/MemoryManager.h"

namespace BaldLion
{
	namespace Rendering
	{
		class DebugDrawRenderProvider {

		public:

			static DebugDrawRenderInterface* Create() 
			{
				switch (RendererPlatformInterface::GetAPI())
				{
				case RendererPlatformInterface::RendererPlatform::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
				case RendererPlatformInterface::RendererPlatform::OpenGL:	return MemoryManager::New<OpenGLDebugRenderInterface>("Debug Draw Render", AllocationType::FreeList_Renderer);
				}

				BL_CORE_ASSERT(false, "Unknown RenderAPI!");
				return nullptr;
			}

			static void Destroy(DebugDrawRenderInterface* debugDrawRender)
			{
				MemoryManager::Delete(debugDrawRender);
			}
		};
	}
}
