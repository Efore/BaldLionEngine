#include "blpch.h"
#include "ECSRenderSystem.h"
#include "BaldLion/Rendering/Renderer.h"

#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/ECS/Components/ECSMeshComponent.h"
#include "BaldLion/ECS/Components/ECSSkeletonComponent.h"
#include "BaldLion/ECS/ComponentsSingleton/ECSProjectionCameraSingleton.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace BaldLion {

	namespace ECS {

		void ECSRenderSystem::OnUpdate()
		{
			ECS::SingletonComponents::ECSProjectionCameraSingleton::UpdateFrustrumPlanes();
			ECSSystem::OnUpdate();
		}

		void ECSRenderSystem::UpdateComponents(ECSComponentLookUp* componentLookUp)
		{
			BL_PROFILE_FUNCTION();

			const ECSTransformComponent* meshTransform = componentLookUp->Read<ECSTransformComponent>(ECSComponentType::Transform);
			const ECSMeshComponent* meshComponent = componentLookUp->Read<ECSMeshComponent>(ECSComponentType::Mesh);			
			const ECSSkeletonComponent* skeletonComponent = componentLookUp->Read<ECSSkeletonComponent>(ECSComponentType::Skeleton);

			if (meshComponent->indices.Size() == 0)
				return;

			const glm::mat4 meshTransformMatrix = meshTransform->GetTransformMatrix();

			const BoundingBox meshAABB = GeometryUtils::GetAABB(meshComponent->localBoundingBox, meshTransformMatrix);
			
			if (ECS::SingletonComponents::ECSProjectionCameraSingleton::IsAABBVisible(meshAABB))
			{					
				if (meshComponent->isStatic)
				{		
					DynamicArray<Vertex> transformedVertices(AllocationType::Linear_Frame, meshComponent->vertices.Size());
					const glm::mat4 meshTransformMatrix = meshTransform->GetTransformMatrix();

					BL_DYNAMICARRAY_FOR(i, meshComponent->vertices, 0)					
					{
						transformedVertices.EmplaceBack(meshComponent->vertices[i] * meshTransformMatrix);
					}

					if (m_parallelize)
					{
						Renderer::ParallelAddStaticMeshToBatch(meshComponent->material, transformedVertices, meshComponent->indices);
					}
					else 
					{
						Renderer::AddStaticMeshToBatch(meshComponent->material, transformedVertices, meshComponent->indices);
					}
				}
				else
				{		
					if (m_parallelize)
					{
						Renderer::ParallelAddDynamicMesh(meshComponent, meshTransform, skeletonComponent);
					}
					else 
					{
						Renderer::AddDynamicMesh(meshComponent, meshTransform, skeletonComponent);
					}
				}
			}	

			if (meshComponent->material->GetCastShadows())
			{	
				if (m_parallelize)
				{
					Renderer::ParallelAddShadowCastingMesh(meshComponent, meshTransform, skeletonComponent);
				}
				else 
				{
					Renderer::AddShadowCastingMesh(meshComponent, meshTransform, skeletonComponent);
				}
			}
		}
	}
}
