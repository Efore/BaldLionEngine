#include "blpch.h"
#include "ECSManager.h"
#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/ECS/Components/ECSProjectionCameraComponent.h"
#include "BaldLion/ECS/Components/ECSMeshComponent.h"
#include "BaldLion/ECS/Components/ECSDirectionalLightComponent.h"
#include "BaldLion/ECS/Components/ECSAnimationComponent.h"
#include "BaldLion/ECS/Components/ECSSkeletonComponent.h"

namespace BaldLion {

	namespace ECS {

		//---------------------------------------------------------------------------------------------------

		ECSManager::ECSManager()
		{
			m_entityComponents = HashMap<ECSEntityID, ECSComponentLookUp>(AllocationType::FreeList_ECS,100);
			m_entitySignatures = HashMap<ECSEntityID, ECSSignature>(AllocationType::FreeList_ECS, 100);
			
			m_systems = DynamicArray<ECSSystem*>(AllocationType::FreeList_ECS,100);

			m_componentsPool = HashTable<ECSComponentID, void*>(AllocationType::FreeList_ECS, 100);

			//Pools initialization
			m_transformComponentPool = DynamicArray<ECSTransformComponent>(AllocationType::FreeList_ECS, 1000);
			m_componentsPool.Emplace(ECSComponentID::Transform, std::move(&m_transformComponentPool));

			m_projectionCameraComponentPool = DynamicArray<ECSProjectionCameraComponent>(AllocationType::FreeList_ECS, 10);
			m_componentsPool.Emplace(ECSComponentID::ProjectionCamera, std::move(&m_projectionCameraComponentPool));

			m_directionalLightComponentPool = DynamicArray<ECSDirectionalLightComponent>(AllocationType::FreeList_ECS, 10);
			m_componentsPool.Emplace(ECSComponentID::DirectionalLight, std::move(&m_directionalLightComponentPool));

			m_meshComponentPool = DynamicArray<ECSMeshComponent>(AllocationType::FreeList_ECS, 40);
			m_componentsPool.Emplace(ECSComponentID::Mesh, std::move(&m_meshComponentPool));

			m_animationComponentPool = DynamicArray<ECSAnimationComponent>(AllocationType::FreeList_ECS, 40);
			m_componentsPool.Emplace(ECSComponentID::Animation, std::move(&m_animationComponentPool));

			m_skeletonComponentPool = DynamicArray<ECSSkeletonComponent>(AllocationType::FreeList_ECS, 40);
			m_componentsPool.Emplace(ECSComponentID::Skeleton, std::move(&m_skeletonComponentPool));
		}

		ECSManager::~ECSManager()
		{
			m_entityComponents.Delete();
			m_entitySignatures.Delete();

			for (ui32 i = 0; i < m_systems.Size(); ++i)
			{
				MemoryManager::Delete(m_systems[i]);
			}

			m_systems.Delete();			

			CleanComponentPool<ECSTransformComponent>(ECSComponentID::Transform);
			CleanComponentPool<ECSProjectionCameraComponent>(ECSComponentID::ProjectionCamera);
			CleanComponentPool<ECSDirectionalLightComponent>(ECSComponentID::DirectionalLight);
			CleanComponentPool<ECSMeshComponent>(ECSComponentID::Mesh);
			CleanComponentPool<ECSAnimationComponent>(ECSComponentID::Animation);
			CleanComponentPool<ECSSkeletonComponent>(ECSComponentID::Skeleton);

			m_componentsPool.Delete();
		}

		void ECSManager::AddEntity(ECSEntityID entityID)
		{
			BL_ASSERT(!m_entitySignatures.Contains(entityID), "Entity already exists");

			ECSSignature newSignature;
			newSignature.reset();

			m_entitySignatures.Emplace(entityID, std::move(newSignature));

			ECSComponentLookUp newComponentLookUp;
			m_entityComponents.Emplace(entityID, std::move(newComponentLookUp));

			for (ui32 i = 0; i < m_systems.Size(); ++i)
			{
				m_systems[i]->OnEntityModified(m_entitySignatures.Get(entityID));
			}
		}

		void ECSManager::AddComponentToEntity(ECSEntityID entityID, ECSComponent* component)
		{
			const ECSComponentID componentID = component->GetComponentID();

			ECSSignature componentSignature;
			componentSignature.set((ui32)componentID);

			m_entitySignatures.Get(entityID) |= componentSignature;
			m_entityComponents.Get(entityID).Set(componentID, component);
		}

		void ECSManager::AddSystem(ECSSystem* system)
		{
			m_systems.PushBack(system);
		}

		void ECSManager::StartSystems()
		{
			for (ui32 i = 0; i < m_systems.Size(); ++i)
			{
				m_systems[i]->OnStart();
			}
		}

		void ECSManager::UpdateSystems(TimeStep timeStep)
		{
			for (ui32 i = 0; i < m_systems.Size(); ++i)
			{
				m_systems[i]->OnUpdate(timeStep);
			}
		}

		void ECSManager::EndOfFrame()
		{
			for (ui32 i = 0; i < m_systems.Size(); ++i)
			{
				m_systems[i]->OnEndOfFrame();
			}
		}

		void ECSManager::StopSystems()
		{
			for (ui32 i = 0; i < m_systems.Size(); ++i)
			{
				m_systems[i]->OnStop();
			}
		}

		void ECSManager::RemoveEntity(ECSEntityID entityID)
		{
			for (ui32 i = 0; i < m_systems.Size(); ++i)
			{
				m_systems[i]->OnEntityModified(m_entitySignatures.Get(entityID));
			}

			m_entityComponents.Remove(entityID);
			m_entitySignatures.Remove(entityID);
		}

		void ECSManager::RemoveComponentFromEntity(ECSComponentID componentID, ECSEntityID entityID)
		{
			ECSSignature signatureToRemove;
			signatureToRemove.set((ui32)componentID);
			signatureToRemove.flip();

			m_entitySignatures.Get(entityID) &= signatureToRemove;			
			m_entityComponents.Get(entityID).Set(componentID, nullptr);

			for (ui32 i = 0; i < m_systems.Size(); ++i)
			{
				m_systems[i]->OnEntityModified(m_entitySignatures.Get(entityID));
			}
		}

		void ECSManager::RemoveSystem(ECSSystem* system)
		{
			m_systems.RemoveFast(system);
		}
	}
}