#include "blpch.h"
#include "Texture.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace BaldLion {

	Ref<Texture2D> Texture2D::Create(const std::string & path)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLTexture2D>(path);
		}

		BL_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}