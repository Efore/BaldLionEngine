#include "blpch.h"
#include "RendererPlatformInterface.h"

namespace BaldLion
{
	namespace Rendering
	{
		RendererPlatformInterface::RendererPlatform RendererPlatformInterface::s_rendererPlatform = RendererPlatformInterface::RendererPlatform::OpenGL;
	}
}