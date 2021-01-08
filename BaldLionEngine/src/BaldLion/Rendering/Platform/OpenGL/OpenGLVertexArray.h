#pragma once
#include "BaldLion/Core/Containers/DynamicArray.h"
#include "BaldLion/Rendering/VertexArray.h"
#include "BaldLion/Rendering/Platform/OpenGL/OpenGLBuffer.h"

namespace BaldLion
{
	namespace Rendering
	{
		class OpenGLVertexArray : public VertexArray
		{
		public:
			OpenGLVertexArray();
			virtual ~OpenGLVertexArray() override;

			virtual void Bind() const override;
			virtual void Unbind() const override;

			virtual void AddVertexBuffer(VertexBuffer* vertexBuffer) override;
			virtual void AddIndexBuffer(IndexBuffer* indexBuffer) override;

			virtual const DynamicArray<VertexBuffer*>& GetVertexBuffers() const override { return m_vertexBuffers; }
			virtual const IndexBuffer* GetIndexBuffer() const { return m_indexBuffer; }

		private:
			ui32 m_rendererID;
			ui32 m_currentLayoutIndex;

			DynamicArray<VertexBuffer*> m_vertexBuffers;
			IndexBuffer* m_indexBuffer;
		};
	}
}