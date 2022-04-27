#pragma once
#include "Vertex.h"
#include "Shader.h"
#include "BaldLion/Core/Containers/DynamicArray.h"

namespace BaldLion
{
	namespace Rendering
	{
		struct BufferElement
		{
			ShaderDataType Type;
			StringId Name;
			ui32 Size;
			ui32 Offset;
			bool Normalized;

			BufferElement() { }

			BufferElement(ShaderDataType type, const char* name, bool normalized = false)
				: Type(type), Name(BL_STRING_TO_STRINGID(name)), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
			{
			}

			ui32 GetComponentCount() const
			{
				switch (Type)
				{
				case ShaderDataType::Float:					return 1;
				case ShaderDataType::Float2:				return 2;
				case ShaderDataType::Float3:				return 3;
				case ShaderDataType::Float4:				return 4;
				case ShaderDataType::Mat3:					return 3 * 3;
				case ShaderDataType::Mat4:					return 4 * 4;
				case ShaderDataType::Int:					return 1;
				case ShaderDataType::Int2:					return 2;
				case ShaderDataType::Int3:					return 3;
				case ShaderDataType::Int4:					return 4;
				case ShaderDataType::Bool:					return 1;
				}

				BL_CORE_ASSERT(false, "Unknown ShaderDataType!");
				return 0;
			}
		};

		class BufferLayout
		{
		public:
			BufferLayout() {}

			BufferLayout(const std::initializer_list<BufferElement>& elements)				
			{
				m_elements = DynamicArray<BufferElement>(AllocationType::FreeList_Renderer, (ui32)elements.size());

				for (auto& element : elements)
				{
					m_elements.PushBack(element);
				}

				CalculateOffsetsAndStride();
			}
			
			~BufferLayout()
			{
				m_elements.Delete();				
			}

			inline const ui32& GetStride() const { return m_stride; }
			inline const DynamicArray<BufferElement>& GetElements() const { return m_elements; }


		private:

			void CalculateOffsetsAndStride()
			{
				ui32 offset = 0;
				m_stride = 0;

				BL_DYNAMICARRAY_FOR(i, m_elements, 0)				
				{
					m_elements[i].Offset = offset;
					offset += m_elements[i].Size;
					m_stride += m_elements[i].Size;
				}
			}

			DynamicArray<BufferElement> m_elements;
			ui32 m_stride = 0;
		};

		class VertexBuffer
		{
		public:
			//move operator
			virtual ~VertexBuffer() {}

			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;

			virtual const BufferLayout& GetLayout() const = 0;
			virtual void SetLayout(const BufferLayout& layout) = 0;

			static VertexBuffer* Create(const void* vertices, ui32 size);
		};

		class IndexBuffer
		{
		public:
			//move operator
			virtual ~IndexBuffer() {}

			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;

			virtual ui32 GetCount() const = 0;
			static IndexBuffer* Create(const ui32* indices, ui32 count);
		};
	}
}