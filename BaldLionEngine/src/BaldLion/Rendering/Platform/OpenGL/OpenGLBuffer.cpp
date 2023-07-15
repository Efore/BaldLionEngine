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
		OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, ui32 size)
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


		void OpenGLVertexBuffer::SetBufferData(const void* vertices, ui32 size)
		{
			BL_PROFILE_FUNCTION();

			glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
			glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
		}

		//-------------------------------------------------
		//Index buffer
		//-------------------------------------------------
		OpenGLIndexBuffer::OpenGLIndexBuffer(const ui32 * indices, ui32 count) : m_count(count)
		{
			BL_PROFILE_FUNCTION();

			glCreateBuffers(1, &m_rendererID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(ui32), indices, GL_STATIC_DRAW);
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

		void OpenGLIndexBuffer::SetBufferData(const ui32* indices, ui32 count) 
		{
			BL_PROFILE_FUNCTION();

			m_count = count;			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(ui32), indices, GL_STATIC_DRAW);
		}
	}
}
