#pragma once

#include <string>
#include <unordered_map>

#include "BaldLion/Renderer/Shader.h"
#include "BaldLion/Renderer/Buffer.h"

namespace BaldLion
{

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetUniform(const std::string& uniformName, ShaderDataType dataType, const void* uniformIndex);
	private:
		virtual int GetUniformLocation(const std::string& name) const;
	private:

		uint32_t m_rendererID;
		mutable std::unordered_map<std::string, int> m_uniformLocationCache;
	};
}
