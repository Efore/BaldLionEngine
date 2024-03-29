#include "blpch.h"
#include "OpenGLVertexArray.h"

#include <vector>
#include <glad/glad.h>

using namespace BaldLion::Memory;

namespace BaldLion
{
	namespace Rendering
	{
		static GLenum ShaderDataTypeToGLEnum(ShaderDataType type)
		{
			switch (type)
			{
			case ShaderDataType::Float:			return GL_FLOAT;
			case ShaderDataType::Float2:		return GL_FLOAT;
			case ShaderDataType::Float3:		return GL_FLOAT;
			case ShaderDataType::Float4:		return GL_FLOAT;
			case ShaderDataType::Mat3:			return GL_FLOAT;
			case ShaderDataType::Mat4:			return GL_FLOAT;
			case ShaderDataType::Int:			return GL_INT;
			case ShaderDataType::Int2:			return GL_INT;
			case ShaderDataType::Int3:			return GL_INT;
			case ShaderDataType::Int4:			return GL_INT;
			case ShaderDataType::Bool:			return GL_BOOL;
			}

			BL_CORE_ASSERT(false, "Unknown shader data type");

			return 0;
		}

		OpenGLVertexArray::OpenGLVertexArray()
		{
			glCreateVertexArrays(1, &m_rendererID);
			glBindVertexArray(m_rendererID);
			m_currentLayoutIndex = 0;
			m_indexBuffer = nullptr;
			m_vertexBuffers = DynamicArray<VertexBuffer*>(AllocationType::FreeList_Renderer, 2);
		}

		OpenGLVertexArray::~OpenGLVertexArray()
		{
			glDeleteVertexArrays(1, &m_rendererID);
			
			if(m_indexBuffer) MemoryManager::Delete(m_indexBuffer);
			
			BL_DYNAMICARRAY_FOR(i, m_vertexBuffers, 0)			
			{
				MemoryManager::Delete(m_vertexBuffers[i]);
			}

			m_vertexBuffers.DeleteNoDestructor();
		}

		void OpenGLVertexArray::Bind() const
		{
			glBindVertexArray(m_rendererID);
		}

		void OpenGLVertexArray::Unbind() const
		{
			glBindVertexArray(0);
		}

		void OpenGLVertexArray::AddVertexBuffer(VertexBuffer* vertexBuffer)
		{
			BL_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().Size(), "Vertex buffer has no layout!");
			
			glBindVertexArray(m_rendererID);

			const auto& layout = vertexBuffer->GetLayout();

			BL_DYNAMICARRAY_FOR(i, layout.GetElements(), 0)			
			{
				glEnableVertexAttribArray(m_currentLayoutIndex);
				glVertexAttribPointer(m_currentLayoutIndex,
					layout.GetElements()[i].GetComponentCount(),
					ShaderDataTypeToGLEnum(layout.GetElements()[i].Type),
					layout.GetElements()[i].Normalized,
					layout.GetStride(),
					(const void*)layout.GetElements()[i].Offset);

				m_currentLayoutIndex++;
			}

			m_vertexBuffers.PushBack(vertexBuffer);

			glBindVertexArray(0);
		}

		void OpenGLVertexArray::AddIndexBuffer(IndexBuffer* indexBuffer)
		{
			m_indexBuffer = indexBuffer;
		}
	}
}
