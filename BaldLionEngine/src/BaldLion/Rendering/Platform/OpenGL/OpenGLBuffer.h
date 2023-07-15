#pragma once

#include "BaldLion/Rendering/Buffer.h"

namespace BaldLion
{
	namespace Rendering
	{
		class OpenGLVertexBuffer : public VertexBuffer
		{
		public:
			OpenGLVertexBuffer(const void* vertices, ui32 size);
			virtual ~OpenGLVertexBuffer();

			virtual void Bind() const override;
			virtual void Unbind() const override;

			virtual void SetBufferData(const void* vertices, ui32 size) override;

			virtual const BufferLayout& GetLayout() const override { return m_layout; };
			virtual void SetLayout(const BufferLayout& layout) override { m_layout = layout; }

		private:
			ui32 m_rendererID;
			BufferLayout m_layout;
		};

		class OpenGLIndexBuffer : public IndexBuffer
		{
		public:
			OpenGLIndexBuffer(const ui32* indices, ui32 count);
			virtual ~OpenGLIndexBuffer();

			virtual void Bind() const override;
			virtual void Unbind() const override;

			virtual void SetBufferData(const ui32* indices, ui32 count) override;

			inline virtual ui32 GetCount() const override { return m_count; }

		private:
			ui32 m_rendererID;
			ui32 m_count;
		};
	}
}
