
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

		Shader* Shader::Create(const std::string& name, const std::string & vertexSrc, const std::string & fragmentSrc)
		{
			switch (RendererPlatformInterface::GetAPI())
			{
			case RendererPlatformInterface::RendererPlatform::None:		BL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererPlatformInterface::RendererPlatform::OpenGL:	return MemoryManager::New<OpenGLShader>("shader", AllocationType::FreeList_Renderer, name, vertexSrc, fragmentSrc);
			}

			BL_CORE_ASSERT(false, "Unknown renderereAPI!");

			return nullptr;
		}

		std::string ShaderLibrary::GetNameFromPath(const std::string& path)
		{
			// Extracting name from lastpath
			auto lastSlash = path.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			auto lastDot = path.rfind('.');
			auto count = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;

			return path.substr(lastSlash, count);
		}

		void ShaderLibrary::Clear()
		{
			if (m_shaders.size() > 0)
			{
				for (auto shaderIterator = m_shaders.begin(); shaderIterator != m_shaders.end(); ++shaderIterator)
				{
					MemoryManager::Delete(shaderIterator->second);
				}
				m_shaders.clear();
			}
		}

		ShaderLibrary::~ShaderLibrary()
		{
		}

		void ShaderLibrary::Add(Shader* shader)
		{
			auto& name = shader->GetName();
			BL_CORE_ASSERT(!Exists(name), "Shader already exists!");
			m_shaders[name] = shader;
		}

		void ShaderLibrary::Add(const std::string& name, Shader* shader)
		{
			BL_CORE_ASSERT(!Exists(name), "Shader already exists!");
			m_shaders[name] = shader;
		}

		Shader* ShaderLibrary::Load(const std::string& filepath)
		{
			const std::string name = ShaderLibrary::GetNameFromPath(filepath);

			if (Exists(name))
				return m_shaders[name];

			auto shader = Shader::Create(filepath);
			Add(shader);
			return shader;
		}

		Shader* ShaderLibrary::Load(const std::string& name, const std::string& filepath)
		{
			if (Exists(name))
				return m_shaders[name];

			auto shader = Shader::Create(filepath);
			Add(shader);
			return shader;
		}

		bool ShaderLibrary::Exists(const std::string& name) const
		{
			return m_shaders.find(name) != m_shaders.end();
		}
	}

}