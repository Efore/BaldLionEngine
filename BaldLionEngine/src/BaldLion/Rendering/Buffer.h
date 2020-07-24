#pragma once
#include "Vertex.h"
#include "Shader.h"

namespace BaldLion
{
	namespace Rendering
	{
		struct BufferElement
		{
			std::string Name;
			ShaderDataType Type;
			uint32_t Size;
			uint32_t Offset;
			bool Normalized;

			BufferElement() { }

			BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
				: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
			{
			}

			uint32_t GetComponentCount() const
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
				: m_elements(elements)
			{
				CalculateOffsetsAndStride();
			}

			inline const uint32_t& GetStride() const { return m_stride; }
			inline const std::vector<BufferElement>& GetElements() const { return m_elements; }

			std::vector<BufferElement>::iterator begin() { return m_elements.begin(); }
			std::vector<BufferElement>::iterator end() { return m_elements.end(); }
			std::vector<BufferElement>::const_iterator begin() const { return m_elements.begin(); }
			std::vector<BufferElement>::const_iterator end() const { return m_elements.end(); }

		private:

			void CalculateOffsetsAndStride()
			{
				uint32_t offset = 0;
				m_stride = 0;
				for (auto& element : m_elements)
				{
					element.Offset = offset;
					offset += element.Size;
					m_stride += element.Size;
				}
			}

			std::vector<BufferElement> m_elements;
			uint32_t m_stride = 0;
		};

		class VertexBuffer
		{
		public:

			virtual ~VertexBuffer() {}

			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;

			virtual const BufferLayout& GetLayout() const = 0;
			virtual void SetLayout(const BufferLayout& layout) = 0;

			static Ref<VertexBuffer> Create(const void* vertices, uint32_t size);
		};

		class IndexBuffer
		{
		public:
			virtual ~IndexBuffer() {}

			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;

			virtual uint32_t GetCount() const = 0;
			static Ref<IndexBuffer> Create(const uint32_t* indices, uint32_t count);
		};
	}
}