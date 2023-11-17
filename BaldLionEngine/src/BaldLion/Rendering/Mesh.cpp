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
		Mesh::Mesh(Material* material, const std::string& modelPath, const std::string& meshPath, bool isPrimitive) :
			ResourceManagement::Resource(BL_STRING_TO_STRINGID(meshPath), meshPath, ResourceManagement::ResourceType::Mesh),
			m_material(material), 
			m_skeleton(nullptr),
			m_isPrimitive(isPrimitive),
			m_modelPath(BL_STRING_TO_STRINGID(modelPath))
		{	
		}

		Mesh::~Mesh()
		{
			VertexArray::Destroy(m_vertexArray);	
			m_geometryData->DeleteGeometryData();
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
				for(ui32 i = 0; i < (ui32)JointType::Count; ++i)
				{
					m_material->GetShader()->SetUniform(BL_STRING_TO_STRINGID(("u_joints[" + std::to_string(i) + "]")), ShaderDataType::Mat4, &(m_skeleton->GetJoints()[i].jointModelSpaceTransform));
				}
			}

			Renderer::Draw(m_vertexArray, m_material->GetShader(), m_material->GetReceiveShadows(), glm::mat4(1.0f));
			m_material->Unbind();			
		}

		void Mesh::SetVertexBones(const DynamicArray<VertexBone>& vertexBones)
		{
			m_vertexBones = DynamicArray<VertexBone>(AllocationType::FreeList_Renderer, vertexBones);
		}

		ECS::ECSEntityID Mesh::GenerateEntity(ECS::ECSManager* ecsManager, bool isStatic) const
		{
			ECS::ECSEntityID entityID = ecsManager->AddEntity(BL_STRINGID_TO_STR_C(m_resourcePath));
			ECS::ECSEntity* entity = ecsManager->GetEntityMap().Get(entityID);

			ECS::ECSTransformComponent* transformComponent = ecsManager->CreateComponent<ECS::ECSTransformComponent>(
				ECS::ECSComponentType::Transform,
				glm::vec3(0.0f),
				glm::vec3(0.0f),
				glm::vec3(1.0f)
				);

			ECS::ECSMeshComponent* meshComponent = nullptr;
			ECS::ECSSkeletonComponent* skeletonComponent = nullptr;

			GenerateMeshComponent(ecsManager, isStatic, transformComponent, meshComponent, skeletonComponent);
			entity->SetIsStatic(meshComponent->isStatic);

			ecsManager->AddComponentToEntity(entityID, transformComponent);
			ecsManager->AddComponentToEntity(entityID, meshComponent);

			if (skeletonComponent != nullptr)
			{
				ecsManager->AddComponentToEntity(entityID, skeletonComponent);
			}		

			return entityID;
		}

		void Mesh::GenerateMeshComponent(ECS::ECSManager* ecsManager, bool isStatic, const ECS::ECSTransformComponent* transformComponent, ECS::ECSMeshComponent*& meshComponent, ECS::ECSSkeletonComponent*& skeletonComponent) const
		{
			if (m_skeleton != nullptr)
			{
				skeletonComponent = GenerateSkeletonComponent(ecsManager);
				isStatic = false;
			}

			meshComponent = ecsManager->CreateComponent<ECS::ECSMeshComponent>(ECS::ECSComponentType::Mesh, isStatic);

			meshComponent->material = m_material;
			meshComponent->indices = DynamicArray<ui32>(AllocationType::FreeList_ECS, m_geometryData->indices);
			meshComponent->meshResourceID = m_resourceID;
			
			meshComponent->vertices = DynamicArray<Vertex>(AllocationType::FreeList_ECS, m_geometryData->vertices);			
			meshComponent->vertexArray = VertexArray::Create();

			IndexBuffer* indexBuffer = IndexBuffer::Create(meshComponent->indices.Data(), meshComponent->indices.Size());
			VertexBuffer* vertexBuffer = VertexBuffer::Create(meshComponent->vertices.Data(), (ui32)(meshComponent->vertices.Size() * sizeof(Vertex)));

			vertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "vertex_position"},
				{ ShaderDataType::Float3, "vertex_normal"},
				{ ShaderDataType::Float3, "vertex_tangent"},
				{ ShaderDataType::Float2, "vertex_texcoord"}
				});

			meshComponent->vertexArray->AddIndexBuffer(indexBuffer);
			meshComponent->vertexArray->AddVertexBuffer(vertexBuffer);

			if (m_skeleton != nullptr)
			{
				VertexBuffer* boneDataVertexBuffer = VertexBuffer::Create(skeletonComponent->boneData[0].GetFirstElement(),
					(ui32)(skeletonComponent->boneData.Size() * sizeof(VertexBone)));

				boneDataVertexBuffer->SetLayout({
					{ ShaderDataType::Int3,		"vertex_joint_ids" },
					{ ShaderDataType::Float3,	"vertex_joint_weights" }
					});

				meshComponent->vertexArray->AddVertexBuffer(boneDataVertexBuffer);
			}

			glm::vec3 minPointInLocalSpace = meshComponent->vertices[0].position;
			glm::vec3 maxPointInLocalSpace = meshComponent->vertices[0].position;

			{				
				BL_DYNAMICARRAY_FOREACH(meshComponent->vertices)				
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

			meshComponent->localBoundingBox = { minPointInLocalSpace , maxPointInLocalSpace, ((maxPointInLocalSpace + minPointInLocalSpace) * 0.5f) };
		}

		BaldLion::ECS::ECSSkeletonComponent* Mesh::GenerateSkeletonComponent(ECS::ECSManager* ecsManager) const
		{
			ECS::ECSSkeletonComponent* skeletonComponent = ecsManager->CreateComponent<ECS::ECSSkeletonComponent>(ECS::ECSComponentType::Skeleton);
						
			for (ui32 i = 0; i < (ui32)JointType::Count; ++i)
			{
				skeletonComponent->joints[i] = m_skeleton->GetJoints()[i];
			}

			skeletonComponent->boneData = DynamicArray<VertexBone>(AllocationType::FreeList_ECS, m_vertexBones);
			skeletonComponent->skeletonResourceID = m_resourceID;
		
			return skeletonComponent;
		}


	}
}
