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

		void ECSRenderSystem::OnUpdate(TimeStep timeStep)
		{
			ECS::SingletonComponents::ECSProjectionCameraSingleton::UpdateFrustrumPlanes();
			ECSSystem::OnUpdate(timeStep);
		}

		void ECSRenderSystem::UpdateOperation(TimeStep timeStep, ECSComponentLookUp* componentLookUp)
		{
			BL_PROFILE_FUNCTION();

			const ECSTransformComponent* meshTransform = componentLookUp->Read<ECSTransformComponent>(ECSComponentType::Transform);
			const ECSMeshComponent* meshComponent = componentLookUp->Read<ECSMeshComponent>(ECSComponentType::Mesh);			
			const ECSSkeletonComponent* skeletonComponent = componentLookUp->Read<ECSSkeletonComponent>(ECSComponentType::Skeleton);

			const glm::mat4 meshTransformMatrix = meshTransform->GetTransformMatrix();

			const glm::vec3 minPointInWorldSpace = glm::vec3(meshTransformMatrix * glm::vec4(meshComponent->localAABB.minPoint, 1.0f));
			const glm::vec3 maxPointInWorldSpace = glm::vec3(meshTransformMatrix * glm::vec4(meshComponent->localAABB.maxPoint, 1.0f));

			const float minX = minPointInWorldSpace.x < maxPointInWorldSpace.x ? minPointInWorldSpace.x : maxPointInWorldSpace.x;
			const float minY = minPointInWorldSpace.y < maxPointInWorldSpace.y ? minPointInWorldSpace.y : maxPointInWorldSpace.y;
			const float minZ = minPointInWorldSpace.z < maxPointInWorldSpace.z ? minPointInWorldSpace.z : maxPointInWorldSpace.z;

			const float maxX = minPointInWorldSpace.x > maxPointInWorldSpace.x ? minPointInWorldSpace.x : maxPointInWorldSpace.x;
			const float maxY = minPointInWorldSpace.y > maxPointInWorldSpace.y ? minPointInWorldSpace.y : maxPointInWorldSpace.y;
			const float maxZ = minPointInWorldSpace.z > maxPointInWorldSpace.z ? minPointInWorldSpace.z : maxPointInWorldSpace.z;

			const AABB meshAABB = { glm::vec3(minX,minY,minZ), glm::vec3(maxX,maxY,maxZ) };
			
			if (ECS::SingletonComponents::ECSProjectionCameraSingleton::IsAABBVisible(meshAABB))
			{					
				if (skeletonComponent == nullptr && meshComponent->isStatic)
				{							
					Renderer::AddStaticMeshToBatch(meshComponent, meshTransform);					
				}
				else
				{					
					Renderer::AddDynamicMesh(meshComponent, meshTransform, skeletonComponent);
				}
			}	

			if (meshComponent->material->GetCastShadows())
			{				
				Renderer::AddShadowCastingMesh(meshComponent, meshTransform, skeletonComponent);
			}
		}
	}
}
