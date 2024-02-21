#include "blpch.h"
#include "ECSFrustrumCullingSystem.h"
#include "BaldLion/Rendering/Renderer.h"

#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/ECS/Components/ECSMeshComponent.h"
#include "BaldLion/ECS/Components/ECSSkeletonComponent.h"
#include "BaldLion/ECS/SingletonSystems/CameraSystem.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace BaldLion {

	namespace ECS {

		void ECSFrustrumCullingSystem::OnUpdate(float deltaTime)
		{
			BL_PROFILE_FUNCTION();
			ECS::SingletonSystems::CameraSystem::UpdateFrustrumPlanes();
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
			meshComponent->isVisible = ECS::SingletonSystems::CameraSystem::IsAABBVisible(meshAABB);		

			if (meshComponent->isVisible)
			{
				meshComponent->isShadowVisible = true;
			}
			else
			{
				meshComponent->isShadowVisible = ECS::SingletonSystems::CameraSystem::IsAABBShadowVisible(meshAABB);
			}
		}

		void ECSFrustrumCullingSystem::OnEntityModified(ECSSignature entitySignature)
		{
			ECSSystem::OnEntityModified(entitySignature);
			Renderer::ForceRefreshComponentLookUps();
		}

	}
}
