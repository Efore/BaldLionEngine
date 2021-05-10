#include "blpch.h"
#include "SkinnedMesh.h"
#include "Buffer.h"
#include "Renderer.h"

namespace BaldLion
{
	namespace Rendering
	{
		SkinnedMesh::SkinnedMesh(Material* material, GeometryUtils::AABB aabb, const glm::mat4& worldTransform, const DynamicArray<Animation::Joint>& joints)
			: Mesh(material,aabb, worldTransform, false), m_joints(std::move(joints))
		{
			
		}

		SkinnedMesh::~SkinnedMesh()
		{
			VertexArray::Destroy(m_vertexArray);
		}

		void SkinnedMesh::SetUpMesh(const DynamicArray<VertexBoneData>& verticesBoneData, const DynamicArray<Vertex>& vertices, const DynamicArray<ui32>& indices)
		{
			BL_PROFILE_FUNCTION();

			m_geometryData = MemoryManager::New<GeometryData>("Geometry data", AllocationType::FreeList_Renderer, vertices, indices, m_worldTransform);

			m_aabb = RecalculateAABB();
			m_vertexArray = VertexArray::Create();

			//Setting index buffer
			IndexBuffer* indexBuffer = (IndexBuffer::Create(&m_geometryData->indices[0], (ui32)m_geometryData->indices.Size()));
			m_vertexArray->AddIndexBuffer(indexBuffer);

			//Setting vertex buffer
			VertexBuffer* vertexBuffer = VertexBuffer::Create(m_geometryData->vertices[0].GetFirstElement(), (ui32)(m_geometryData->vertices.Size() * sizeof(Vertex)));

			vertexBuffer->SetLayout({
				{ ShaderDataType::Float3,	"vertex_position"},
				{ ShaderDataType::Float3,	"vertex_color"},
				{ ShaderDataType::Float3,	"vertex_normal"},
				{ ShaderDataType::Float3,	"vertex_tangent"},
				{ ShaderDataType::Float2,	"vertex_texcoord"}
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