#include "blpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace BaldLion
{
	namespace Rendering
	{
		RendererAPI* RenderCommand::s_rendererAPI = new OpenGLRendererAPI;
	}
}