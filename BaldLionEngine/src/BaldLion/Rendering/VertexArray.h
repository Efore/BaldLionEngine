#pragma once

#include "BaldLion/Rendering/Buffer.h"
#include "BaldLion/Core/Containers/DynamicArray.h"

namespace BaldLion
{
	namespace Rendering
	{
		class VertexArray
		{
		public:
			virtual ~VertexArray() {}

			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;

			virtual void AddVertexBuffer(VertexBuffer* vertexBuffer) = 0;
			virtual void AddIndexBuffer(IndexBuffer* indexBuffer) = 0;

			virtual const DynamicArray<VertexBuffer*> & GetVertexBuffers() const = 0;
			virtual const IndexBuffer* GetIndexBuffer() const = 0;

			static VertexArray* Create();
			static void Destroy(VertexArray* vertexArray);
		};
	}
}