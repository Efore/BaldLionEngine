#include "blpch.h"
#include "Mesh.h"
#include "Buffer.h"
#include "Renderer.h"

namespace BaldLion
{
	namespace Rendering
	{
		Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const Ref<Material>& material) : 
			m_material(material)
		{
			SetUpMesh(vertices, indices);
		}

		Mesh::~Mesh()
		{

		}

		void Mesh::SetUpMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
		{
			BL_PROFILE_FUNCTION();

			m_vertexArray = VertexArray::Create();

			Ref<IndexBuffer> indexBuffer;
			indexBuffer = (IndexBuffer::Create(&indices[0], (uint32_t)indices.size()));

			Ref<VertexBuffer> vertexBuffer;
			vertexBuffer = VertexBuffer::Create(vertices[0].GetFirstElement(), (uint32_t)(vertices.size() * sizeof(Vertex)));

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

		void Mesh::Draw() const
		{
			m_material->Bind();

			Renderer::Submit(m_vertexArray, m_material->GetShader());
		}
	}
}
