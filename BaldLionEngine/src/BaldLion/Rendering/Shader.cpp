
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
			case RendererPlatformInterface::RendererPlatform::OpenGL:	return MemoryManager::New<OpenGLShader>(STRING_TO_ID(filepath),AllocationType::FreeList_Renderer, filepath);
			}

			BL_CORE_ASSERT(false, "Unknown renderereAPI!");

			return nullptr;
		}

		ShaderLibrary::~ShaderLibrary()
		{
		}

		void ShaderLibrary::Init()
		{
			m_shaders = HashTable<StringId, Shader*>(BaldLion::Memory::AllocationType::FreeList_Renderer, 10);
		}

		void ShaderLibrary::Add(Shader* shader)
		{
			auto name = shader->GetName();
			BL_CORE_ASSERT(!Exists(name), "Shader already exists!");
			m_shaders.Emplace(name , std::move(shader));
		}

		void ShaderLibrary::Add(StringId name, Shader* shader)
		{
			BL_CORE_ASSERT(!Exists(name), "Shader already exists!");
			m_shaders.Emplace(name, std::move(shader));
		}

		Shader* ShaderLibrary::Load(const std::string& filepath)
		{
			StringId name;
			ShaderLibrary::GetNameFromPath(filepath, name);

			return Load(name, filepath);
		}

		Shader* ShaderLibrary::Load(StringId name, const std::string& filepath)
		{
			std::lock_guard<std::mutex> lockGuard(m_shaderLibraryMutex);

			//if (Exists(name))
			//	return m_shaders.Get(name);

			auto shader = Shader::Create(filepath);
			//Add(shader);
			return shader;
		}

		bool ShaderLibrary::Exists(StringId name) const
		{
			return m_shaders.Contains(name);
		}

		void ShaderLibrary::Clear()
		{
			m_shaders.Clear();
		}

		void ShaderLibrary::GetNameFromPath(const std::string &path, StringId& name)
		{
			// Extracting name from lastpath
			auto lastSlash = path.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			auto lastDot = path.rfind('.');
			auto count = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;

			name = STRING_TO_ID(path.substr(lastSlash, count));
		}
	}

}