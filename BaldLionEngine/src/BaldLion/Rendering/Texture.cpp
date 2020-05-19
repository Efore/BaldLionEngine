#include "blpch.h"
#include "Texture.h"

#include "RendererPlatformInterface.h"
#include "Platform/OpenGL/OpenGLTexture2D.h"
#include "Platform/OpenGL/OpenGLTextureCubemap.h"

namespace BaldLion 
{
	namespace Rendering
	{
		Ref<Texture2D> Texture2D::Create(const std::string & path)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
			case RendererPlatformInterface::RendererPlatform::None:			BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererPlatformInterface::RendererPlatform::OpenGL:		return CreateRef<OpenGLTexture2D>(path);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}

		Ref<Texture2D> Texture2D::Create(const std::string& path, const unsigned char* textureData, int size)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
				case RendererPlatformInterface::RendererPlatform::None:			BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
				case RendererPlatformInterface::RendererPlatform::OpenGL:		return CreateRef<OpenGLTexture2D>(path, textureData, size);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}

		Ref<TextureCubeMap> TextureCubeMap::Create(const std::string & path)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
			case RendererPlatformInterface::RendererPlatform::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererPlatformInterface::RendererPlatform::OpenGL:		return CreateRef<OpenGLTextureCubemap>(path);
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

		void TextureLibrary::Add(const Ref<Texture>& texture)
		{
			auto& name = texture->GetName();
			BL_CORE_ASSERT(!Exists(name), "Shader already exists!");
			m_textures[name] = texture;
		}

		void TextureLibrary::Add(const std::string& name, const Ref<Texture>& texture)
		{
			BL_CORE_ASSERT(!Exists(name), "Shader already exists!");
			m_textures[name] = texture;
		}

		Ref<Texture> TextureLibrary::Load(const std::string& filepath, int textureType)
		{	
			const std::string name = TextureLibrary::GetNameFromPath(filepath);
			
			return Load(name,filepath,textureType);
		}

		Ref<Texture> TextureLibrary::Load(const std::string& name, const std::string& filepath, int textureType)
		{	
			if (Exists(name))
				return Get(name);

			Ref<Texture> texture = nullptr;

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

		BaldLion::Ref<BaldLion::Rendering::Texture> TextureLibrary::Load(const std::string& filepath, const unsigned char* textureData, int size, int textureType)
		{
			const std::string name = TextureLibrary::GetNameFromPath(filepath);

			if (Exists(name))
				return Get(name);

			Ref<Texture> texture = nullptr;

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

		Ref<Texture> TextureLibrary::Get(const std::string& name)
		{
			BL_CORE_ASSERT(Exists(name), "Texture not found!");
			return m_textures[name];
		}

		bool TextureLibrary::Exists(const std::string& name) const
		{
			return m_textures.find(name) != m_textures.end();
		}

		std::string TextureLibrary::GetNameFromPath(const std::string &path)
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