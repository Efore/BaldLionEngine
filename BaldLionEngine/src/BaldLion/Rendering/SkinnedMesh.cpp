#include "blpch.h"
#include "SkinnedMesh.h"
#include "Buffer.h"
#include "Renderer.h"

namespace BaldLion
{
	namespace Rendering
	{
		SkinnedMesh::SkinnedMesh(Material* material, GeometryUtils::AABB aabb, const DynamicArray<Vertex>& vertices, const DynamicArray<ui32>& indices, const glm::mat4& worldTransform, const DynamicArray<Animation::Joint>& joints)
			: Mesh(material,aabb, vertices, indices, worldTransform), m_joints(std::move(joints))
		{
			
		}

		SkinnedMesh::~SkinnedMesh()
		{
			VertexArray::Destroy(m_vertexArray);
		}

		void SkinnedMesh::SetUpMesh(const DynamicArray<VertexBoneData>& verticesBoneData)
		{
			OPTICK_EVENT();

			m_aabb = RecalculateAABB();
			m_vertexArray = VertexArray::Create();

			//Setting index buffer
			IndexBuffer* indexBuffer = (IndexBuffer::Create(&m_indices[0], (ui32)m_indices.Size()));
			m_vertexArray->AddIndexBuffer(indexBuffer);

			//Setting vertex buffer
			VertexBuffer* vertexBuffer = VertexBuffer::Create(m_vertices[0].GetFirstElement(), (ui32)(m_vertices.Size() * sizeof(Vertex)));

			vertexBuffer->SetLayout({
				{ ShaderDataType::Float3,	"vertex_position"},
				{ ShaderDataType::Float3,	"vertex_color"},
				{ ShaderDataType::Float3,	"vertex_normal"},
				{ ShaderDataType::Float2,	"vertex_texcoord"},
				{ ShaderDataType::Float3,	"vertex_tangent"},
				{ ShaderDataType::Float3,	"vertex_bitangent"}
			});

			m_vertexArray->AddVertexBuffer(vertexBuffer);

			//Setting bone data vertex buffer
			VertexBuffer* boneDataVertexBuffer = VertexBuffer::Create(verticesBoneData[0].GetFirstElement(), (ui32)(verticesBoneData.Size() * sizeof(VertexBoneData)));

			boneDataVertexBuffer->SetLayout({
				{ ShaderDataType::Int3,		"vertex_joint_ids" },
				{ ShaderDataType::Float3,	"vertex_joint_weights" }
			});

			m_vertexArray->AddVertexBuffer(boneDataVertexBuffer);
		}

		void SkinnedMesh::Draw() const
		{
			m_material->Bind();		

			for (ui32 i = 0; i < m_joints.Size(); ++i)
			{
				m_material->GetShader()->SetUniform(STRING_TO_ID(("u_joints[" + std::to_string(i) + "]")), ShaderDataType::Mat4, &(m_joints[i].jointAnimationTransform));
			}

			Renderer::Draw(m_vertexArray, m_material->GetShader(), m_worldTransform);		
		}

	}
}