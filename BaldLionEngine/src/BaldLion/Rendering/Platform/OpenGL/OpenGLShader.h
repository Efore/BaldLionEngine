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
			OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
			~OpenGLShader();

			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual const std::string& GetName() const override { return m_name; }

			virtual void SetUniform(const std::string& uniformName, ShaderDataType dataType, const void* uniformIndex) override;

		private:

			virtual int GetUniformLocation(const std::string& name) const;
			std::string ReadFile(const std::string& filepath);
			std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
			void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

		private:

			uint32_t m_rendererID;
			mutable std::unordered_map<std::string, int> m_uniformLocationCache;
			std::string m_name;
		};
	}
}
