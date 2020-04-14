#include "blpch.h"
#include "AnimatedMesh.h"
#include "Renderer.h"

namespace BaldLion
{
	namespace Rendering
	{

		AnimatedMesh::AnimatedMesh(std::vector<Vertex> vertices, std::vector<VertexBoneData> verticesBoneData, std::vector<Animation::Joint> joints, std::vector<uint32_t> indices, const Ref<Material>& material)
			: Mesh(vertices, indices, material), m_verticesBoneData(verticesBoneData), m_joints(joints)
		{
			
		}

		AnimatedMesh::~AnimatedMesh()
		{

		}

		void AnimatedMesh::SetUpMesh()
		{
			Mesh::SetUpMesh();

		}

		void AnimatedMesh::Draw() const
		{
			m_material->Bind();

			Renderer::Submit(m_vertexArray, m_material->GetShader());
		}
	}
}