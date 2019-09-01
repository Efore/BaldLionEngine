#include "blpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace BaldLion
{
	RendererAPI* RenderCommand::s_rendererAPI = new OpenGLRendererAPI;
}