
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
			case RendererPlatformInterface::RendererPlatform::OpenGL:	return MemoryManager::New<OpenGLShader>("shader",AllocationType::FreeList_Renderer, filepath);
			}

			BL_CORE_ASSERT(false, "Unknown renderereAPI!");

			return nullptr;
		}

		ShaderLibrary::~ShaderLibrary()
		{
		}

		void ShaderLibrary::Init()
		{
			m_shaders = HashTable<const char*, Shader*>(BaldLion::Memory::AllocationType::FreeList_Renderer, 10);
		}

		void ShaderLibrary::Add(Shader* shader)
		{
			auto name = shader->GetName();
			BL_CORE_ASSERT(!Exists(name), "Shader already exists!");
			m_shaders.Insert(name , std::move(shader));
		}

		void ShaderLibrary::Add(const char* name, Shader* shader)
		{
			BL_CORE_ASSERT(!Exists(name), "Shader already exists!");
			m_shaders.Insert(name, std::move(shader));
		}

		Shader* ShaderLibrary::Load(const std::string& filepath)
		{
			char name[100];
			ShaderLibrary::GetNameFromPath(filepath, name);

			return Load(name, filepath);
		}

		BaldLion::Rendering::Shader* ShaderLibrary::Load(const char* name, const std::string& filepath)
		{
			if (Exists(name))
				return m_shaders.Get(name);

			auto shader = Shader::Create(filepath);
			Add(shader);
			return shader;
		}

		bool ShaderLibrary::Exists(const char* name) const
		{
			return m_shaders.Contains(name);
		}

		void ShaderLibrary::Clear()
		{
			m_shaders.Clear();
		}

		void ShaderLibrary::GetNameFromPath(const std::string &path, char *name)
		{
			// Extracting name from lastpath
			auto lastSlash = path.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			auto lastDot = path.rfind('.');
			auto count = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;

			strcpy(name, path.substr(lastSlash, count).c_str());
		}
	}

}