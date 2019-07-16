#pragma once

#include <string>
#include "Buffer.h"

namespace BaldLion 
{

	class Shader
	{
	public:
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;

		void SetUniform(const std::string& uniformName, ShaderDataType dataType, const void* uniformIndex);
	private:

		uint32_t m_rendererID;
	};
}
