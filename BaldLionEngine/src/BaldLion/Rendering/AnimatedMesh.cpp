#include "blpch.h"
#include "AnimatedMesh.h"
#include "Buffer.h"
#include "Renderer.h"

namespace BaldLion
{
	namespace Rendering
	{

		AnimatedMesh::AnimatedMesh(std::vector<AnimatedVertex> vertices, std::vector<uint32_t> indices, std::vector<Animation::Joint> joints, const Ref<Material>& material)
			:m_vertices(vertices), m_indices(indices), m_material(material), m_joints(joints)
		{
			SetUpMesh();
		}

		AnimatedMesh::~AnimatedMesh()
		{

		}

		void AnimatedMesh::SetUpMesh()
		{
			BL_PROFILE_FUNCTION();

			m_vertexArray = VertexArray::Create();

			Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(&m_vertices[0], (uint32_t)(m_vertices.size() * sizeof(AnimatedVertex)));

			vertexBuffer->SetLayout({
				{ ShaderDataType::Float3,	"vertex_position"},
				{ ShaderDataType::Float3,	"vertex_color"},
				{ ShaderDataType::Float3,	"vertex_normal"},
				{ ShaderDataType::Float2,	"vertex_texcoord"},
				{ ShaderDataType::Float3,	"vertex_tangent"},
				{ ShaderDataType::Float3,	"vertex_bitangent"},
				{ ShaderDataType::Int3,		"vertex_joint_ids"},
				{ ShaderDataType::Float3,	"vertex_joint_weights"}
			});

			Ref<IndexBuffer> indexBuffer;
			indexBuffer = (IndexBuffer::Create(&m_indices[0], (uint32_t)m_indices.size()));

			m_vertexArray->AddIndexBuffer(indexBuffer);
			m_vertexArray->AddVertexBuffer(vertexBuffer);
		}

		void AnimatedMesh::Draw() const
		{
			m_material->Bind();

			m_material->GetShader()->Bind();

			for (size_t i = 0; i < m_joints.size(); ++i)
			{
				m_material->GetShader()->SetUniform("u_joints[" + std::to_string(i) + "]", ShaderDataType::Mat4, &(m_joints[i].jointAnimationTransform));
			}

			Renderer::Submit(m_vertexArray, m_material->GetShader());		
		}

		glm::mat4 AnimatedMesh::AiMat4ToGlmMat4(const aiMatrix4x4& aiMat4)
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