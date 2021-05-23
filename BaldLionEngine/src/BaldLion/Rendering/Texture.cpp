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
			case RendererPlatformInterface::RendererPlatform::OpenGL:		return MemoryManager::New<OpenGLTexture2D>(path.c_str(), AllocationType::FreeList_Renderer, path, emptyTexture);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}

		Texture2D* Texture2D::Create(const std::string& path, const unsigned char* textureData, int size)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
				case RendererPlatformInterface::RendererPlatform::None:			BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
				case RendererPlatformInterface::RendererPlatform::OpenGL:		return  MemoryManager::New<OpenGLTexture2D>(path.c_str(), AllocationType::FreeList_Renderer, path, textureData, size);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}

		TextureCubeMap* TextureCubeMap::Create(const std::string & path)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
			case RendererPlatformInterface::RendererPlatform::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererPlatformInterface::RendererPlatform::OpenGL:	return MemoryManager::New<OpenGLTextureCubemap>(path.c_str(), AllocationType::FreeList_Renderer, path);
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
		
		TextureLibrary::~TextureLibrary()
		{			
		}

		void TextureLibrary::Init()
		{
			m_textures = HashTable<StringId, Texture*>(BaldLion::Memory::AllocationType::FreeList_Renderer, 10);
		}

		void TextureLibrary::Add(Texture* texture)
		{
			auto name = texture->GetName();
			BL_CORE_ASSERT(!Exists(name), "Shader already exists!");
			m_textures.Emplace(name, std::move(texture));
		}

		void TextureLibrary::Add(StringId name, Texture* texture)
		{
			BL_CORE_ASSERT(!Exists(name), "Shader already exists!");
			m_textures.Emplace(name, std::move(texture));
		}

		Texture* TextureLibrary::Load(const std::string& filepath, TextureType textureType)
		{	
			StringId name;
			TextureLibrary::GetNameFromPath(filepath, name);
			
			return Load(name,filepath,textureType);
		}

		Texture* TextureLibrary::Load(StringId name, const std::string& filepath, TextureType textureType)
		{	
			std::lock_guard<std::mutex> lockGuard(m_textureLibraryMutex);

			if (Exists(name))
				return m_textures.Get(name);

			Texture* texture = nullptr;

			switch (textureType)
			{
				case TextureType::Texture2d:
					texture = Texture2D::Create(filepath);
					break;

				case TextureType::CubeMap:
					texture = TextureCubeMap::Create(filepath);
					break;

				default:
					texture = Texture2D::Create(filepath);
					break;
			}

			Add(texture);
			return texture;
		}

		Texture* TextureLibrary::Load(const std::string& filepath, const unsigned char* textureData, int size, TextureType textureType)
		{
			std::lock_guard<std::mutex> lockGuard(m_textureLibraryMutex);

			StringId name;
			TextureLibrary::GetNameFromPath(filepath, name);

			if (Exists(name))
				return m_textures.Get(name);

			Texture* texture = nullptr;

			switch (textureType)
			{
			case TextureType::Texture2d:
				texture = Texture2D::Create(filepath, textureData, size);
				break;

			case TextureType::CubeMap:
				texture = TextureCubeMap::Create(filepath);
				break;

			default:
				texture = Texture2D::Create(filepath, textureData, size);
				break;
			}

			Add(texture);
			return texture;
		}

		void TextureLibrary::Clear()
		{
			std::lock_guard<std::mutex> lockGuard(m_textureLibraryMutex);
			m_textures.Clear();
		}

		bool TextureLibrary::Exists(StringId name) const
		{
			return m_textures.Contains(name);
		}

		void TextureLibrary::GetNameFromPath(const std::string &path, StringId& name)
		{
			// Extracting name from last path
			auto lastSlash = path.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			auto lastDot = path.rfind('.');
			auto count = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;
			name = STRING_TO_ID(path.substr(lastSlash, count));			
		}

	}
}