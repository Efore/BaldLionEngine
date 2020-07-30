#include "blpch.h"

#include "Framebuffer.h"
#include "RendererPlatformInterface.h"
#include "BaldLion/Rendering/Platform/OpenGL/OpenGLFramebuffer.h"

namespace BaldLion
{
	namespace Rendering
	{
		Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& specs)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
			case RendererPlatformInterface::RendererPlatform::None:			BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererPlatformInterface::RendererPlatform::OpenGL:		return CreateRef<OpenGLFramebuffer>(specs);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}
	}
}