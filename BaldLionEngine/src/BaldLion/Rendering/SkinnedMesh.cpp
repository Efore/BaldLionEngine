#include "blpch.h"
#include "SkinnedMesh.h"
#include "Buffer.h"
#include "Renderer.h"

namespace BaldLion
{
	namespace Rendering
	{
		SkinnedMesh::SkinnedMesh(const DynamicArray<Animation::Joint>& joints, Material* material)
			: m_material(material), m_joints(std::move(joints))
		{
			
		}

		SkinnedMesh::~SkinnedMesh()
		{
			VertexArray::Destroy(m_vertexArray);
			Material::Destroy(m_material);
		}

		void SkinnedMesh::SetUpMesh(const DynamicArray<Vertex>& vertices, const DynamicArray<VertexBoneData>& verticesBoneData, const DynamicArray<ui32>& indices)
		{
			BL_PROFILE_FUNCTION();

			m_vertexArray = VertexArray::Create();

			//Setting index buffer
			IndexBuffer* indexBuffer = (IndexBuffer::Create(&indices[0], (ui32)indices.Size()));
			m_vertexArray->AddIndexBuffer(indexBuffer);

			//Setting vertex buffer
			VertexBuffer* vertexBuffer = VertexBuffer::Create(vertices[0].GetFirstElement(), (ui32)(vertices.Size() * sizeof(Vertex)));

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

		void SkinnedMesh::Draw(const glm::mat4& worldTransform) const
		{
			m_material->Bind();

			m_material->GetShader()->Bind();

			for (ui32 i = 0; i < m_joints.Size(); ++i)
			{
				m_material->GetShader()->SetUniform(STRING_TO_ID(("u_joints[" + std::to_string(i) + "]")), ShaderDataType::Mat4, &(m_joints[i].jointAnimationTransform));
			}

			Renderer::Submit(m_vertexArray, m_material->GetShader(), worldTransform);		
		}

		glm::mat4 SkinnedMesh::AiMat4ToGlmMat4(const aiMatrix4x4& aiMat4)
		{
			return glm::mat4(
				aiMat4.a1, aiMat4.b1, aiMat4.c1, aiMat4.d1,
				aiMat4.a2, aiMat4.b2, aiMat4.c2, aiMat4.d2,
				aiMat4.a3, aiMat4.b3, aiMat4.c3, aiMat4.d3,
				aiMat4.a4, aiMat4.b4, aiMat4.c4, aiMat4.d4
			);
		}
	}
}