#pragma once

#include "BaldLion/Rendering/Shader.h"
#include "BaldLion/Rendering/Buffer.h"

#include <glm/glm.hpp>

typedef unsigned int GLenum;

namespace BaldLion
{
	namespace Rendering
	{
		class OpenGLShader : public Shader
		{
		public:
			OpenGLShader(const std::string& filepath);
			~OpenGLShader();

			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual const char* GetName() const override { return m_name; }

			virtual void SetUniform(const char* uniformName, ShaderDataType dataType, const void* uniformIndex) override;

		private:

			virtual int GetUniformLocation(const char* name) const;
			std::string ReadFile(const std::string& filepath);
			std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
			void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

		private:

			uint32_t m_rendererID;
			mutable HashTable<const char*, int> m_uniformLocationCache;
			char m_name[100];
		};
	}
}
