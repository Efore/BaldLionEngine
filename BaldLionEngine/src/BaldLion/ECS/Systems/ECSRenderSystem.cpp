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

			const ECSTransformComponent* meshTransform = componentLookUp->Read<ECSTransformComponent>(ECSComponentID::Transform);
			const ECSMeshComponent* meshComponent = componentLookUp->Read<ECSMeshComponent>(ECSComponentID::Mesh);			
			const ECSSkeletonComponent* skeletonComponent = componentLookUp->Read<ECSSkeletonComponent>(ECSComponentID::Skeleton);

			const glm::mat4 meshTransformMatrix = meshTransform->GetTransformMatrix();

			glm::vec3 minPointInWorldSpace = glm::vec3(meshTransformMatrix * glm::vec4(meshComponent->vertices[0].position, 1.0f));
			glm::vec3 maxPointInWorldSpace = glm::vec3(meshTransformMatrix * glm::vec4(meshComponent->vertices[0].position, 1.0f));

			
			for (ui32 i = 0; i < meshComponent->vertices.Size(); ++i)
			{
				const glm::vec3 vertexPosInWorldSpace = glm::vec3(meshTransformMatrix * glm::vec4(meshComponent->vertices[i].position, 1.0f));

				if (vertexPosInWorldSpace.x > maxPointInWorldSpace.x)	maxPointInWorldSpace.x = vertexPosInWorldSpace.x;
				if (vertexPosInWorldSpace.y > maxPointInWorldSpace.y)	maxPointInWorldSpace.y = vertexPosInWorldSpace.y;
				if (vertexPosInWorldSpace.z > maxPointInWorldSpace.z)	maxPointInWorldSpace.z = vertexPosInWorldSpace.z;

				if (vertexPosInWorldSpace.x < minPointInWorldSpace.x)	minPointInWorldSpace.x = vertexPosInWorldSpace.x;
				if (vertexPosInWorldSpace.y < minPointInWorldSpace.y)	minPointInWorldSpace.y = vertexPosInWorldSpace.y;
				if (vertexPosInWorldSpace.z < minPointInWorldSpace.z)	minPointInWorldSpace.z = vertexPosInWorldSpace.z;
			}
			

			const AABB meshAABB = { minPointInWorldSpace, maxPointInWorldSpace };
			
			if (ECS::SingletonComponents::ECSProjectionCameraSingleton::IsAABBVisible(meshAABB))
			{	
				BL_PROFILE_SCOPE("Distributing mesh data", Optick::Category::Rendering);
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
