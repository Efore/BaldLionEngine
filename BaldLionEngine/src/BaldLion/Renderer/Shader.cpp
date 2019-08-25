
#include "blpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace BaldLion
{
	Ref<Shader> Shader::Create(const std::string & vertexSrc, const std::string & fragmentSrc)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLShader>(vertexSrc,fragmentSrc);
		}

		BL_CORE_ASSERT(false, "Unknown renderereAPI!");

		return nullptr;
	}

}