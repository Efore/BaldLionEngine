
#include "blpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace BaldLion
{
	OpenGLShader::OpenGLShader(const std::string & vertexSrc, const std::string & fragmentSrc)
	{
		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar *source = vertexSrc.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(vertexShader);

			// Use the infoLog as you see fit.
			BL_LOG_CORE_ERROR("{0}", infoLog.data());
			BL_CORE_ASSERT(false, "Vertex shader compilation failure!");
			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = fragmentSrc.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		// Compile the fragment shader
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(fragmentShader);

			// Either of them. Don't leak shaders.
			glDeleteShader(vertexShader);

			// Use the infoLog as you see fit.
			BL_LOG_CORE_ERROR("{0}", infoLog.data());
			BL_CORE_ASSERT(false, "Fragment shader compilation failure!");
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		m_rendererID = glCreateProgram();
		GLuint program = m_rendererID;

		// Attach our shaders to our program
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

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
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);


			// Use the infoLog as you see fit.
			BL_LOG_CORE_ERROR("{0}", infoLog.data());
			BL_CORE_ASSERT(false, "Shader link failure!");
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_rendererID);
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