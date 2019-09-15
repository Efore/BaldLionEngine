#include "blpch.h"
#include "OpenGLShader.h"

#include <fstream>
#include <glad/glad.h>

namespace BaldLion
{
	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")		
			return GL_VERTEX_SHADER;

		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		BL_CORE_ASSERT(false, "Unknown shader type!");
		return GL_FALSE;
	}

	OpenGLShader::OpenGLShader(const std::string & filepath)
	{
		std::string source = ReadFile(filepath);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);

		// Extracting name from lastpath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;

		m_name = filepath.substr(lastSlash, count);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string & vertexSrc, const std::string & fragmentSrc) 
		: m_name (name)
	{
		std::unordered_map<GLenum, std::string> shaderSources;
		shaderSources[GL_VERTEX_SHADER] = vertexSrc;
		shaderSources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(shaderSources);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_rendererID);
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		std::string result;

		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			BL_LOG_CORE_ERROR("Shader not found in path {0}", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string & source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokeLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);

		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			BL_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokeLength + 1;
			std::string type = source.substr(begin, eol - begin);
			BL_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type");

			size_t nextLinePos = source.find_first_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos,
				pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{		
		GLuint program = glCreateProgram();
		BL_CORE_ASSERT(shaderSources.size() <= 2, "Only 2 shaders supported");

		std::array<GLenum, 2> glShaderIDs;
		int glShaderIDsIndex = 0;

		for (auto &kv : shaderSources)
		{
			GLenum shaderType = kv.first;
			const std::string& shaderCode = kv.second;

			GLuint shader = glCreateShader(shaderType);

			const GLchar *sourceCStr = shaderCode.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			// Compile the vertex shader
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shader);

				// Use the infoLog as you see fit.
				BL_LOG_CORE_ERROR("{0}", infoLog.data());
				BL_CORE_ASSERT(false, "Shader compilation failure!");
				return;
			}

			glAttachShader(program, shader);
			glShaderIDs[glShaderIDsIndex++] = shader;
		}

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);

			// Don't leak shaders either.
			for (auto id : glShaderIDs)
			{
				glDeleteShader(id);
			}			

			// Use the infoLog as you see fit.
			BL_LOG_CORE_ERROR("{0}", infoLog.data());
			BL_CORE_ASSERT(false, "Shader link failure!");
			return;
		}

		for (auto id : glShaderIDs)		
			glDetachShader(program, id);				

		m_rendererID = program;
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_rendererID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::SetUniform(const std::string& uniformName, ShaderDataType dataType, const void* uniformIndex)
	{
		int uniformID = GetUniformLocation(uniformName);

		switch (dataType)
		{
		case ShaderDataType::Int:
			glUniform1i(uniformID, (int&)uniformIndex);
			return;

		case ShaderDataType::Float:
			glUniform1f(uniformID, (float&)uniformIndex);
			return;

		case ShaderDataType::Float2:
			glUniform2f(uniformID, ((glm::vec2*)uniformIndex)->x, ((glm::vec2*)uniformIndex)->y);
			return;

		case ShaderDataType::Float3:
			glUniform3f(uniformID, ((glm::vec3*)uniformIndex)->x, ((glm::vec3*)uniformIndex)->y, ((glm::vec3*)uniformIndex)->z);
			return;

		case ShaderDataType::Float4:
			glUniform4f(uniformID, ((glm::vec4*)uniformIndex)->x, ((glm::vec4*)uniformIndex)->y, ((glm::vec4*)uniformIndex)->z, ((glm::vec4*)uniformIndex)->w);
			return;

		case ShaderDataType::Mat4:
			glUniformMatrix4fv(uniformID, 1, GL_FALSE, (float*)uniformIndex);
			return;
		}
	}

	int OpenGLShader::GetUniformLocation(const std::string& name) const
	{
		if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
			return m_uniformLocationCache[name];

		int location = glGetUniformLocation(m_rendererID, name.c_str());

		if (location == -1)
		{
			BL_LOG_CORE_WARN("Uniform '{0}' not found!", name);
			return location;
		}

		m_uniformLocationCache[name] = location;

		return location;
	}

}