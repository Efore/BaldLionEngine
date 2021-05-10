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
			virtual StringId GetName() const override { return m_name; }

			virtual void SetUniform(StringId uniformName, ShaderDataType dataType, const void* uniformIndex) override;

		private:

			virtual int GetUniformLocation(StringId name) const;
			std::string ReadFile(const std::string& filepath);
			std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
			void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

		private:

			ui32 m_rendererID;
			StringId m_name;
			mutable HashTable<StringId, int> m_uniformLocationCache;
		};
	}
}
