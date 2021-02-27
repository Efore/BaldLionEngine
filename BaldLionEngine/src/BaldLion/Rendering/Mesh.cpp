#include "blpch.h"
#include "Mesh.h"
#include "Buffer.h"
#include "Renderer.h"

namespace BaldLion
{
	namespace Rendering
	{
		Mesh::Mesh(Material* material) :
			m_material(material)
		{			
		}

		Mesh::~Mesh()
		{
			VertexArray::Destroy(m_vertexArray);
			Material::Destroy(m_material);
		}

		void Mesh::SetUpMesh(const DynamicArray<Vertex>& vertices, const DynamicArray<ui32>& indices)
		{
			BL_PROFILE_FUNCTION();

			m_vertexArray = VertexArray::Create();

			IndexBuffer* indexBuffer = IndexBuffer::Create(&indices[0], (ui32)indices.Size());

			VertexBuffer* vertexBuffer = VertexBuffer::Create(vertices[0].GetFirstElement(), (ui32)(vertices.Size() * sizeof(Vertex)));

			vertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "vertex_position"},
				{ ShaderDataType::Float3, "vertex_color"},
				{ ShaderDataType::Float3, "vertex_normal"},
				{ ShaderDataType::Float2, "vertex_texcoord"},
				{ ShaderDataType::Float3, "vertex_tangent"},
				{ ShaderDataType::Float3, "vertex_bitangent"},
			});

			m_vertexArray->AddIndexBuffer(indexBuffer);
			m_vertexArray->AddVertexBuffer(vertexBuffer);
		}

		void Mesh::Draw(const glm::mat4& worldTransform) const
		{
			m_material->Bind();
			Renderer::Submit(m_vertexArray, m_material->GetShader(), worldTransform);
		}
	}
}
