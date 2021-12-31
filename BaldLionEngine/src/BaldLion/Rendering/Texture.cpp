#include "blpch.h"
#include "Texture.h"

#include "RendererPlatformInterface.h"
#include "Platform/OpenGL/OpenGLTexture2D.h"
#include "Platform/OpenGL/OpenGLTextureCubemap.h"

namespace BaldLion 
{
	namespace Rendering
	{
		Texture2D* Texture2D::Create(const std::string& path, bool emptyTexture)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
			case RendererPlatformInterface::RendererPlatform::None:			BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererPlatformInterface::RendererPlatform::OpenGL:		return MemoryManager::New<OpenGLTexture2D>(("Texture " + path).c_str(), AllocationType::FreeList_Renderer, path, emptyTexture);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}

		Texture2D* Texture2D::Create(const std::string& path, const unsigned char* textureData, int size)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
				case RendererPlatformInterface::RendererPlatform::None:			BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
				case RendererPlatformInterface::RendererPlatform::OpenGL:		return  MemoryManager::New<OpenGLTexture2D>(("Texture " + path).c_str(), AllocationType::FreeList_Renderer, path, textureData, size);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}

		TextureCubeMap* TextureCubeMap::Create(const std::string & path)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
			case RendererPlatformInterface::RendererPlatform::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererPlatformInterface::RendererPlatform::OpenGL:	return MemoryManager::New<OpenGLTextureCubemap>(("CubemapTexture " + path).c_str(), AllocationType::FreeList_Renderer, path);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}

		const std::string TextureCubeMap::GetSkyboxTexturePath(const std::string& path, size_t index)
		{
			// Extracting name from lastpath			
			auto lastDot = path.rfind('.');

			const std::string extension = path.substr(lastDot, path.size() - 1);
			const std::string subpath = path.substr(0, lastDot);

			char textureIndex[3];
			sprintf(textureIndex, "_%d", (int)index);

			return subpath + textureIndex + extension;
		}
		
	}
}