#include "blpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace BaldLion
{
	namespace Rendering
	{
		//-------------------------------------------------
		//Vertex buffer
		//-------------------------------------------------
		OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
		{
			BL_PROFILE_FUNCTION();

			glCreateBuffers(1, &m_rendererID);
			glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
			glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
		}

		OpenGLVertexBuffer::~OpenGLVertexBuffer()
		{
			BL_PROFILE_FUNCTION();

			glDeleteBuffers(1, &m_rendererID);
		}


		void OpenGLVertexBuffer::Bind() const
		{
			BL_PROFILE_FUNCTION();

			glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		}

		void OpenGLVertexBuffer::Unbind() const
		{
			BL_PROFILE_FUNCTION();

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}


		//-------------------------------------------------
		//Index buffer
		//-------------------------------------------------
		OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t * indices, uint32_t count) : m_count(count)
		{
			BL_PROFILE_FUNCTION();

			glCreateBuffers(1, &m_rendererID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
		}

		OpenGLIndexBuffer::~OpenGLIndexBuffer()
		{
			BL_PROFILE_FUNCTION();

			glDeleteBuffers(1, &m_rendererID);
		}

		void OpenGLIndexBuffer::Bind() const
		{
			BL_PROFILE_FUNCTION();

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
		}

		void OpenGLIndexBuffer::Unbind() const
		{
			BL_PROFILE_FUNCTION();

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}
}