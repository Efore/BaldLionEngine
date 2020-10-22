#include "blpch.h"
#include "Texture.h"

#include "RendererPlatformInterface.h"
#include "Platform/OpenGL/OpenGLTexture2D.h"
#include "Platform/OpenGL/OpenGLTextureCubemap.h"

namespace BaldLion 
{
	namespace Rendering
	{
		Texture2D* Texture2D::Create(const std::string & path)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
			case RendererPlatformInterface::RendererPlatform::None:			BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererPlatformInterface::RendererPlatform::OpenGL:		return MemoryManager::New<OpenGLTexture2D>("texture", AllocationType::FreeList_Renderer, path);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}

		Texture2D* Texture2D::Create(const std::string& path, const unsigned char* textureData, int size)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
				case RendererPlatformInterface::RendererPlatform::None:			BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
				case RendererPlatformInterface::RendererPlatform::OpenGL:		return  MemoryManager::New<OpenGLTexture2D>("texture", AllocationType::FreeList_Renderer, path, textureData, size);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}

		TextureCubeMap* TextureCubeMap::Create(const std::string & path)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
			case RendererPlatformInterface::RendererPlatform::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererPlatformInterface::RendererPlatform::OpenGL:	return MemoryManager::New<OpenGLTextureCubemap>("texture", AllocationType::FreeList_Renderer, path);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}

		const std::string TextureCubeMap::GetSkyboxTexturePath(const std::string& path, int index)
		{
			// Extracting name from lastpath			
			auto lastDot = path.rfind('.');

			const std::string extension = path.substr(lastDot, path.size() - 1);
			const std::string subpath = path.substr(0, lastDot);

			char textureIndex[3];
			sprintf(textureIndex, "_%d", index);

			return subpath + textureIndex + extension;
		}

		TextureLibrary::~TextureLibrary()
		{
			for (auto textureIterator = m_textures.begin(); textureIterator != m_textures.end(); ++textureIterator)
			{
				MemoryManager::Delete(textureIterator->second);
			}
			m_textures.clear();
		}

		void TextureLibrary::Add(Texture* texture)
		{
			auto& name = texture->GetName();
			BL_CORE_ASSERT(!Exists(name), "Shader already exists!");
			m_textures[name] = texture;
		}

		void TextureLibrary::Add(const std::string& name, Texture* texture)
		{
			BL_CORE_ASSERT(!Exists(name), "Shader already exists!");
			m_textures[name] = texture;
		}

		Texture* TextureLibrary::Load(const std::string& filepath, int textureType)
		{	
			const std::string& name = TextureLibrary::GetNameFromPath(filepath);
			
			return Load(name,filepath,textureType);
		}

		Texture* TextureLibrary::Load(const std::string& name, const std::string& filepath, int textureType)
		{	
			if (Exists(name))
				return m_textures[name];

			Texture* texture = nullptr;

			switch (textureType)
			{
				case BL_TEXTURE_TYPE_2D:
					texture = Texture2D::Create(filepath);
					break;

				case BL_TEXTURE_TYPE_CUBEMAP:
					texture = TextureCubeMap::Create(filepath);
					break;

				default:
					texture = Texture2D::Create(filepath);
					break;
			}

			Add(texture);
			return texture;
		}

		Texture* TextureLibrary::Load(const std::string& filepath, const unsigned char* textureData, int size, int textureType)
		{
			const std::string name = TextureLibrary::GetNameFromPath(filepath);

			if (Exists(name))
				return m_textures[name];

			Texture* texture = nullptr;

			switch (textureType)
			{
			case BL_TEXTURE_TYPE_2D:
				texture = Texture2D::Create(filepath, textureData, size);
				break;

			case BL_TEXTURE_TYPE_CUBEMAP:
				texture = TextureCubeMap::Create(filepath);
				break;

			default:
				texture = Texture2D::Create(filepath, textureData, size);
				break;
			}

			Add(texture);
			return texture;
		}


		bool TextureLibrary::Exists(const std::string& name) const
		{
			return m_textures.find(name) != m_textures.end();
		}

		const std::string TextureLibrary::GetNameFromPath(const std::string &path)
		{
			// Extracting name from lastpath
			auto lastSlash = path.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			auto lastDot = path.rfind('.');
			auto count = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;

			return path.substr(lastSlash, count);
		}

	}
}