#include "blpch.h"
#include "Texture.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace BaldLion 
{
	namespace Rendering
	{
		Ref<Texture2D> Texture2D::Create(const std::string & path)
		{
			switch (RendererAPI::GetAPI())
			{
			case RendererAPI::API::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererAPI::API::OpenGL:		return CreateRef<OpenGLTexture2D>(path);
			}

			BL_CORE_ASSERT(false, "Unknown RenderAPI!");
			return nullptr;
		}

		void TextureLibrary::Add(const Ref<Texture2D>& texture)
		{
			auto& name = texture->GetName();
			BL_CORE_ASSERT(!Exists(name), "Shader already exists!");
			m_textures[name] = texture;
		}

		void TextureLibrary::Add(const std::string& name, const Ref<Texture2D>& texture)
		{
			BL_CORE_ASSERT(!Exists(name), "Shader already exists!");
			m_textures[name] = texture;
		}

		Ref<Texture2D> TextureLibrary::Load(const std::string& filepath)
		{	
			const std::string name = TextureLibrary::GetNameFromPath(filepath);
			
			if (Exists(name))
				return Get(name);

			auto texture = Texture2D::Create(filepath);
			Add(texture);
			return texture;
		}

		Ref<Texture2D> TextureLibrary::Load(const std::string& name, const std::string& filepath)
		{	
			if (Exists(name))
				return Get(name);

			auto texture = Texture2D::Create(filepath);
			Add(texture);
			return texture;

		}

		Ref<Texture2D> TextureLibrary::Get(const std::string& name)
		{
			BL_CORE_ASSERT(Exists(name), "Shader not found!");
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