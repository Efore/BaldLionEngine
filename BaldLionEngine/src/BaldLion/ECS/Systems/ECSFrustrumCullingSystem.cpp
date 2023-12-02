#include "blpch.h"
#include "ECSFrustrumCullingSystem.h"
#include "BaldLion/Rendering/Renderer.h"

#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/ECS/Components/ECSMeshComponent.h"
#include "BaldLion/ECS/Components/ECSSkeletonComponent.h"
#include "BaldLion/ECS/ComponentsSingleton/CameraSystem.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace BaldLion {

	namespace ECS {

		void ECSFrustrumCullingSystem::OnUpdate(float deltaTime)
		{
			ECS::SingletonComponents::CameraSystem::UpdateFrustrumPlanes();
			ECSSystem::OnUpdate(deltaTime);
		}

		void ECSFrustrumCullingSystem::UpdateComponents(ECSEntityID entityID, ECSComponentLookUp* componentLookUp, float deltaTime)
		{
			BL_PROFILE_FUNCTION();

			const ECSTransformComponent* meshTransform = componentLookUp->Read<ECSTransformComponent>(ECSComponentType::Transform);
			ECSMeshComponent* meshComponent = componentLookUp->Write<ECSMeshComponent>(ECSComponentType::Mesh);			
			const ECSSkeletonComponent* skeletonComponent = componentLookUp->Read<ECSSkeletonComponent>(ECSComponentType::Skeleton);

			if (meshComponent->indices.Size() == 0)
				return;

			const glm::mat4 meshTransformMatrix = meshTransform->GetTransformMatrix();		
			
			const BoundingBox meshAABB = GeometryUtils::GetAABB(meshComponent->localBoundingBox, meshTransformMatrix);
			meshComponent->isVisible = ECS::SingletonComponents::CameraSystem::IsAABBVisible(meshAABB);
			if (meshComponent->isVisible)
			{
				Renderer::AddMeshToDraw(meshComponent, meshTransform, skeletonComponent);
			}
			

			//if (meshComponent->material->GetCastShadows())
			//{	
			//	Renderer::AddShadowCastingMesh(meshComponent, meshTransform, skeletonComponent);				
			//}
		}
	}
}
