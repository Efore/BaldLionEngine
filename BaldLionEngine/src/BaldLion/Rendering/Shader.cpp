
#include "blpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace BaldLion
{
	namespace Rendering
	{
		Shader* Shader::Create(const std::string & filepath)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
			case RendererPlatformInterface::RendererPlatform::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererPlatformInterface::RendererPlatform::OpenGL:	return MemoryManager::New<OpenGLShader>(("Shader " + filepath).c_str(), AllocationType::FreeList_Renderer, filepath);
			}

			BL_CORE_ASSERT(false, "Unknown renderereAPI!");

			return nullptr;
		}
	}

}