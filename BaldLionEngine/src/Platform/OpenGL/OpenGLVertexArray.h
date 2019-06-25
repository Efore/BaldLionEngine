#pragma once

#include "BaldLion/Renderer/VertexArray.h"

namespace BaldLion
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
		virtual void AddIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<std::shared_ptr<VertexBuffer>> & GetVertexBuffer() const { return m_vertexBuffers; }
		virtual const std::shared_ptr<IndexBuffer> & GetIndexBuffer() const { return m_indexBuffer; }
		
	private:
		uint32_t m_rendererID;
		std::vector<std::shared_ptr<VertexBuffer>> m_vertexBuffers;
		std::shared_ptr<IndexBuffer> m_indexBuffer;
	};
}