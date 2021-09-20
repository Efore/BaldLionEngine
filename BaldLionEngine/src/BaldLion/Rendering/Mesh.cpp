#include "blpch.h"
#include "Mesh.h"
#include "Buffer.h"
#include "Renderer.h"
#include <glm/gtc/quaternion.hpp>
#include "BaldLion/ECS/ECSManager.h"

namespace BaldLion
{
	namespace Rendering
	{
		Mesh::Mesh(Material* material) :
			m_material(material)
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
		}

		void Mesh::Draw() const
		{			
			m_material->Bind();
			Renderer::Draw(m_vertexArray, m_material->GetShader(), m_material->GetReceiveShadows(), glm::mat4(1.0f));
			m_material->Unbind();			
		}

		ECS::ECSMeshComponent* Mesh::GenerateMeshComponent(ECS::ECSManager* ecsManager, bool isStatic)
		{
			ECS::ECSMeshComponent* meshComponent = ecsManager->AddComponent<ECS::ECSMeshComponent>(ECS::ECSComponentID::Mesh, isStatic);

			meshComponent->material = m_material;
			meshComponent->vertices = DynamicArray<Vertex>(AllocationType::FreeList_ECS, m_geometryData->vertices);
			meshComponent->indices = DynamicArray<ui32>(AllocationType::FreeList_ECS, m_geometryData->indices);

			return meshComponent;
		}

	}
}
