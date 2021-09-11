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
			
			if (ECS::SingletonComponents::ECSProjectionCameraSingleton::IsAABBVisible(meshComponent->aabb))
			{
				if (skeletonComponent == nullptr && meshComponent->isStatic)
				{
					std::lock_guard<std::mutex> frustrumCullingGuard(Renderer::s_geometryToBatchMutex);
					Renderer::AddStaticMeshToBatch(meshComponent, meshTransform);					
				}
				else
				{
					std::lock_guard<std::mutex> frustrumCullingGuard(Renderer::s_dynamicMeshesToRenderMutex);
					Renderer::AddDynamicMesh(meshComponent, meshTransform, skeletonComponent);
				}
			}	

			if (meshComponent->material->GetCastShadows())
			{
				std::lock_guard<std::mutex> frustrumCullingGuard(Renderer::s_shadowCastingMeshesMutex);
				Renderer::AddShadowCastingMesh(meshComponent, meshTransform, skeletonComponent);
			}
		}

	}
}
