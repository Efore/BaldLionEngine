#include "blpch.h"
#include "Mesh.h"
#include "Buffer.h"
#include "Renderer.h"
#include <glm/gtc/quaternion.hpp>

namespace BaldLion
{
	namespace Rendering
	{
		Mesh::Mesh(Material* material, GeometryUtils::AABB aabb, const glm::mat4& worldTransform, bool isStatic) :
			m_material(material), m_aabb(aabb), m_worldTransform(worldTransform), m_isStatic(isStatic)
		{	
		}

		Mesh::~Mesh()
		{
			VertexArray::Destroy(m_vertexArray);	
			m_geometryData->ClearGeometryData();
			MemoryManager::Delete(m_geometryData);
		}

		void Mesh::SetUpMesh(const DynamicArray<Vertex>& vertices, const DynamicArray<ui32>& indices)
		{
			BL_PROFILE_FUNCTION();

			m_geometryData = MemoryManager::New<GeometryData>("Geometry data", AllocationType::FreeList_Renderer, vertices, indices, m_worldTransform);

			m_aabb = RecalculateAABB();

			m_vertexArray = VertexArray::Create();

			IndexBuffer* indexBuffer = IndexBuffer::Create(&m_geometryData->indices[0], (ui32)m_geometryData->indices.Size());

			VertexBuffer* vertexBuffer = VertexBuffer::Create(m_geometryData->vertices[0].GetFirstElement(), (ui32)(m_geometryData->vertices.Size() * sizeof(Vertex)));

			vertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "vertex_position"},
				{ ShaderDataType::Float3, "vertex_color"},
				{ ShaderDataType::Float3, "vertex_normal"},
				{ ShaderDataType::Float3, "vertex_tangent"},
				{ ShaderDataType::Float2, "vertex_texcoord"}
				});

			m_vertexArray->AddIndexBuffer(indexBuffer);
			m_vertexArray->AddVertexBuffer(vertexBuffer);
			
		}

		void Mesh::Draw() const
		{	
			if (!m_isStatic) {

				m_material->Bind();
				Renderer::Draw(m_vertexArray, m_material->GetShader(), m_material->GetReceiveShadows(), m_worldTransform);
				m_material->Unbind();
			}
		}

		#define FAST_AABB 0

		AABB Mesh::RecalculateAABB() const
		{

			#if FAST_AABB

			DynamicArray<glm::vec3> bbCorners = DynamicArray<glm::vec3>(AllocationType::Linear_Frame, 8);

			bbCorners.EmplaceBack(m_aabb.maxPoint);
			bbCorners.EmplaceBack(glm::vec3(m_aabb.maxPoint.x, m_aabb.maxPoint.y, m_aabb.minPoint.z));
			bbCorners.EmplaceBack(glm::vec3(m_aabb.maxPoint.x, m_aabb.minPoint.y, m_aabb.maxPoint.z));
			bbCorners.EmplaceBack(glm::vec3(m_aabb.maxPoint.x, m_aabb.minPoint.y, m_aabb.minPoint.z));
			bbCorners.EmplaceBack(glm::vec3(m_aabb.minPoint.x, m_aabb.maxPoint.y, m_aabb.maxPoint.z));
			bbCorners.EmplaceBack(glm::vec3(m_aabb.minPoint.x, m_aabb.maxPoint.y, m_aabb.minPoint.z));
			bbCorners.EmplaceBack(glm::vec3(m_aabb.minPoint.x, m_aabb.minPoint.y, m_aabb.maxPoint.z));
			bbCorners.EmplaceBack(m_aabb.minPoint);

			glm::vec3 minPointInWorldSpace = glm::vec3(glm::vec4(bbCorners[0], 1.0f));
			glm::vec3 maxPointInWorldSpace = glm::vec3(glm::vec4(bbCorners[0], 1.0f));

			for (ui32 i = 0; i < bbCorners.Size(); ++i)
			{
				const glm::vec3 vertexPosInWorldSpace = m_worldTransform * glm::vec4(bbCorners[i], 1.0f);

				if (vertexPosInWorldSpace.x > maxPointInWorldSpace.x)	maxPointInWorldSpace.x = vertexPosInWorldSpace.x;
				if (vertexPosInWorldSpace.y > maxPointInWorldSpace.y)	maxPointInWorldSpace.y = vertexPosInWorldSpace.y;
				if (vertexPosInWorldSpace.z > maxPointInWorldSpace.z)	maxPointInWorldSpace.z = vertexPosInWorldSpace.z;

				if (vertexPosInWorldSpace.x < minPointInWorldSpace.x)	minPointInWorldSpace.x = vertexPosInWorldSpace.x;
				if (vertexPosInWorldSpace.y < minPointInWorldSpace.y)	minPointInWorldSpace.y = vertexPosInWorldSpace.y;
				if (vertexPosInWorldSpace.z < minPointInWorldSpace.z)	minPointInWorldSpace.z = vertexPosInWorldSpace.z;
			}

			#else

			glm::vec3 minPointInWorldSpace = glm::vec3(m_worldTransform * glm::vec4(m_geometryData->vertices[0].position, 1.0f));
			glm::vec3 maxPointInWorldSpace = glm::vec3(m_worldTransform * glm::vec4(m_geometryData->vertices[0].position, 1.0f));

			for(ui32 i = 0; i < m_geometryData->vertices.Size(); ++i)
			{ 
				const glm::vec3 vertexPosInWorldSpace = glm::vec3(m_worldTransform * glm::vec4(m_geometryData->vertices[i].position, 1.0f));

				if (vertexPosInWorldSpace.x > maxPointInWorldSpace.x)	maxPointInWorldSpace.x = vertexPosInWorldSpace.x;
				if (vertexPosInWorldSpace.y > maxPointInWorldSpace.y)	maxPointInWorldSpace.y = vertexPosInWorldSpace.y;
				if (vertexPosInWorldSpace.z > maxPointInWorldSpace.z)	maxPointInWorldSpace.z = vertexPosInWorldSpace.z;

				if (vertexPosInWorldSpace.x < minPointInWorldSpace.x)	minPointInWorldSpace.x = vertexPosInWorldSpace.x;
				if (vertexPosInWorldSpace.y < minPointInWorldSpace.y)	minPointInWorldSpace.y = vertexPosInWorldSpace.y;
				if (vertexPosInWorldSpace.z < minPointInWorldSpace.z)	minPointInWorldSpace.z = vertexPosInWorldSpace.z;
			}

			#endif

			return { minPointInWorldSpace, maxPointInWorldSpace };
		}

	}
}
