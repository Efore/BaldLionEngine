#include "blpch.h"
#include "Mesh.h"
#include "Buffer.h"
#include "Renderer.h"

#include <string_view>

namespace BaldLion
{
	namespace Rendering
	{
		Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, const Ref<Material>& material) : m_vertices(vertices), m_indices(indices), m_material(material)
		{
			SetUpMesh();
		}

		Mesh::~Mesh()
		{

		}

		void Mesh::SetUpMesh()
		{
			Ref<VertexBuffer> vertexBuffer;
			vertexBuffer = VertexBuffer::Create(&m_vertices[0], (uint32_t)(m_vertices.size() * sizeof(Vertex)));

			vertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "vertex_position"},
				{ ShaderDataType::Float3, "vertex_color"},
				{ ShaderDataType::Float3, "vertex_normal"},
				{ ShaderDataType::Float2, "vertex_texcoord"}
				});

			m_vertexArray = VertexArray::Create();
			m_vertexArray->AddVertexBuffer(vertexBuffer);

			Ref<IndexBuffer> indexBuffer;
			indexBuffer = (IndexBuffer::Create(&m_indices[0], (uint32_t)m_indices.size()));
			m_vertexArray->AddIndexBuffer(indexBuffer);

			m_material->Bind();
		}

		void Mesh::Draw() const
		{
			m_material->Bind();

			Renderer::Submit(m_vertexArray, m_material->GetShader());
		}
	}
}
