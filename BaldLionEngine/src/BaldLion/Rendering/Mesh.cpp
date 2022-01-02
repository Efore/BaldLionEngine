#include "blpch.h"
#include "Mesh.h"
#include "Buffer.h"
#include "Renderer.h"
#include <glm/gtc/quaternion.hpp>
#include "BaldLion/ECS/ECSManager.h"
#include "BaldLion/Animation/Skeleton.h"

namespace BaldLion
{
	namespace Rendering
	{
		Mesh::Mesh(Material* material, const std::string& meshPath) :
			ResourceManagement::Resource(STRING_TO_STRINGID(meshPath), StringUtils::GetFileNameFromPath(meshPath), ResourceManagement::ResourceType::Mesh),
			m_material(material), 
			m_skeleton(nullptr)
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

			m_geometryData = MemoryManager::New<GeometryData>("Geometry data", AllocationType::FreeList_Renderer, vertices, indices);

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

			if (m_skeleton != nullptr)
			{
				VertexBuffer* boneDataVertexBuffer = VertexBuffer::Create(m_vertexBones[0].GetFirstElement(),
					(ui32)(m_vertexBones.Size() * sizeof(VertexBone)));

				boneDataVertexBuffer->SetLayout({
					{ ShaderDataType::Int3,		"vertex_joint_ids" },
					{ ShaderDataType::Float3,	"vertex_joint_weights" }
					});

				m_vertexArray->AddVertexBuffer(boneDataVertexBuffer);
			}
		}

		void Mesh::Draw() const
		{			
			m_material->Bind();

			if (m_skeleton != nullptr)
			{
				for (ui32 i = 0; i < m_skeleton->GetJoints().Size(); ++i)
				{
					m_material->GetShader()->SetUniform(STRING_TO_STRINGID(("u_joints[" + std::to_string(i) + "]")), ShaderDataType::Mat4, &(m_skeleton->GetJoints()[i].jointAnimationTransform));
				}
			}

			Renderer::Draw(m_vertexArray, m_material->GetShader(), m_material->GetReceiveShadows(), glm::mat4(1.0f));
			m_material->Unbind();			
		}

		void Mesh::SetVertexBones(const DynamicArray<VertexBone>& vertexBones)
		{
			m_vertexBones = DynamicArray<VertexBone>(AllocationType::FreeList_Renderer, vertexBones);
		}

		ECS::ECSMeshComponent* Mesh::GenerateMeshComponent(ECS::ECSManager* ecsManager, bool isStatic)
		{
			ECS::ECSMeshComponent* meshComponent = ecsManager->AddComponent<ECS::ECSMeshComponent>(ECS::ECSComponentType::Mesh, isStatic);

			meshComponent->material = m_material;
			meshComponent->vertices = DynamicArray<Vertex>(AllocationType::FreeList_ECS, m_geometryData->vertices);
			meshComponent->indices = DynamicArray<ui32>(AllocationType::FreeList_ECS, m_geometryData->indices);
			meshComponent->meshResourceID = m_resourceID;

			glm::vec3 minPointInLocalSpace = meshComponent->vertices[0].position;
			glm::vec3 maxPointInLocalSpace = meshComponent->vertices[0].position;

			{				
				for (ui32 i = 0; i < meshComponent->vertices.Size(); ++i)
				{
					const glm::vec3 vertexPosInWorldSpace = meshComponent->vertices[i].position;

					if (vertexPosInWorldSpace.x > maxPointInLocalSpace.x)	maxPointInLocalSpace.x = vertexPosInWorldSpace.x;
					if (vertexPosInWorldSpace.y > maxPointInLocalSpace.y)	maxPointInLocalSpace.y = vertexPosInWorldSpace.y;
					if (vertexPosInWorldSpace.z > maxPointInLocalSpace.z)	maxPointInLocalSpace.z = vertexPosInWorldSpace.z;

					if (vertexPosInWorldSpace.x < minPointInLocalSpace.x)	minPointInLocalSpace.x = vertexPosInWorldSpace.x;
					if (vertexPosInWorldSpace.y < minPointInLocalSpace.y)	minPointInLocalSpace.y = vertexPosInWorldSpace.y;
					if (vertexPosInWorldSpace.z < minPointInLocalSpace.z)	minPointInLocalSpace.z = vertexPosInWorldSpace.z;
				}
			}

			meshComponent->localAABB = { minPointInLocalSpace , maxPointInLocalSpace };

			return meshComponent;
		}

		BaldLion::ECS::ECSSkeletonComponent* Mesh::GenerateSkeletonComponent(ECS::ECSManager* ecsManager)
		{
			ECS::ECSSkeletonComponent* skeletonComponent = ecsManager->AddComponent<ECS::ECSSkeletonComponent>(ECS::ECSComponentType::Skeleton);

			skeletonComponent->joints = DynamicArray<Animation::Joint>(AllocationType::FreeList_ECS, m_skeleton->GetJoints());
			skeletonComponent->boneData = DynamicArray<VertexBone>(AllocationType::FreeList_ECS, m_vertexBones);
			skeletonComponent->skeletonResourceID = m_resourceID;
		
			return skeletonComponent;
		}

	}
}
