#include "blpch.h"
#include "ECSManager.h"
#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/ECS/Components/ECSProjectionCameraComponent.h"
#include "BaldLion/ECS/Components/ECSMeshComponent.h"
#include "BaldLion/ECS/Components/ECSDirectionalLightComponent.h"
#include "BaldLion/ECS/Components/ECSAnimationComponent.h"
#include "BaldLion/ECS/Components/ECSSkeletonComponent.h"
#include "BaldLion/ECS/Components/ECSHierarchyComponent.h"

namespace BaldLion {

	namespace ECS {

		//---------------------------------------------------------------------------------------------------

		ECSManager::ECSManager()
		{
			m_entityComponents = HashMap<ECSEntityID, ECSComponentLookUp>(AllocationType::FreeList_ECS,100);
			m_entitySignatures = HashMap<ECSEntityID, ECSSignature>(AllocationType::FreeList_ECS, 100);
			m_entitiyMap = HashMap<ECSEntityID, ECSEntity*>(AllocationType::FreeList_ECS, 100);

			m_entities = DynamicArray<ECSEntity>(AllocationType::FreeList_ECS,100);			
			m_systems = DynamicArray<ECSSystem*>(AllocationType::FreeList_ECS,100);

			m_componentsPool = HashTable<ECSComponentType, void*>(AllocationType::FreeList_ECS, 100);

			//Pools initialization
			m_transformComponentPool = DynamicArray<ECSTransformComponent>(AllocationType::FreeList_ECS, 1000);
			m_componentsPool.Emplace(ECSComponentType::Transform, std::move(&m_transformComponentPool));

			m_projectionCameraComponentPool = DynamicArray<ECSProjectionCameraComponent>(AllocationType::FreeList_ECS, 10);
			m_componentsPool.Emplace(ECSComponentType::ProjectionCamera, std::move(&m_projectionCameraComponentPool));

			m_directionalLightComponentPool = DynamicArray<ECSDirectionalLightComponent>(AllocationType::FreeList_ECS, 10);
			m_componentsPool.Emplace(ECSComponentType::DirectionalLight, std::move(&m_directionalLightComponentPool));

			m_meshComponentPool = DynamicArray<ECSMeshComponent>(AllocationType::FreeList_ECS, 40);
			m_componentsPool.Emplace(ECSComponentType::Mesh, std::move(&m_meshComponentPool));

			m_animationComponentPool = DynamicArray<ECSAnimationComponent>(AllocationType::FreeList_ECS, 40);
			m_componentsPool.Emplace(ECSComponentType::Animation, std::move(&m_animationComponentPool));

			m_skeletonComponentPool = DynamicArray<ECSSkeletonComponent>(AllocationType::FreeList_ECS, 40);
			m_componentsPool.Emplace(ECSComponentType::Skeleton, std::move(&m_skeletonComponentPool));

			m_hierarchyComponentPool = DynamicArray<ECSHierarchyComponent>(AllocationType::FreeList_ECS, 40);
			m_componentsPool.Emplace(ECSComponentType::Hierarchy, std::move(&m_hierarchyComponentPool));

			m_entityIDProvider = 0;
		}

		ECSManager::~ECSManager()
		{
			m_entityComponents.Delete();
			m_entitySignatures.Delete();
			m_entities.Delete();

			for (ui32 i = 0; i < m_systems.Size(); ++i)
			{
				MemoryManager::Delete(m_systems[i]);
			}

			m_systems.Delete();			

			CleanComponentPool<ECSTransformComponent>(ECSComponentType::Transform);
			CleanComponentPool<ECSProjectionCameraComponent>(ECSComponentType::ProjectionCamera);
			CleanComponentPool<ECSDirectionalLightComponent>(ECSComponentType::DirectionalLight);
			CleanComponentPool<ECSMeshComponent>(ECSComponentType::Mesh);
			CleanComponentPool<ECSAnimationComponent>(ECSComponentType::Animation);
			CleanComponentPool<ECSSkeletonComponent>(ECSComponentType::Skeleton);
			CleanComponentPool<ECSHierarchyComponent>(ECSComponentType::Hierarchy);

			m_componentsPool.Delete();
		}

		ECSEntityID ECSManager::AddEntity(const char* entityName)
		{
			ECSEntityID entityID = m_entityIDProvider++;

			ECSEntity newEntity(entityName, entityID);

			m_entities.PushBack(std::move(newEntity));
			m_entitiyMap.Emplace(entityID, &m_entities[m_entities.Size() - 1]);

			ECSSignature newSignature;
			newSignature.reset();

			m_entitySignatures.Emplace(entityID, std::move(newSignature));

			ECSComponentLookUp newComponentLookUp;
			m_entityComponents.Emplace(entityID, std::move(newComponentLookUp));			

			for (ui32 i = 0; i < m_systems.Size(); ++i)
			{
				m_systems[i]->OnEntityModified(m_entitySignatures.Get(entityID));
			}

			return entityID;
		}

		void ECSManager::AddComponentToEntity(ECSEntityID entityID, ECSComponent* component)
		{
			const ECSComponentType componentID = component->GetComponentType();

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

		void ECSManager::FrameStart()
		{
			for (ui32 i = 0; i < m_systems.Size(); ++i)
			{
				m_systems[i]->OnFrameStart();
			}
		}

		void ECSManager::UpdateSystems(TimeStep timeStep)
		{
			for (ui32 i = 0; i < m_systems.Size(); ++i)
			{
				m_systems[i]->OnUpdate(timeStep);
			}
		}

		void ECSManager::FrameEnd()
		{
			for (ui32 i = 0; i < m_systems.Size(); ++i)
			{
				m_systems[i]->OnFrameEnd();
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

			for (ui32 i = 0; i < m_entities.Size(); ++i)
			{				
				if (m_entities[i].GetEntityID() == entityID) {
					m_entities.RemoveAt(i);
					break;
				}				
			}
			
			m_entitiyMap.Remove(entityID);
			m_entityComponents.Remove(entityID);
			m_entitySignatures.Remove(entityID);
		}

		void ECSManager::RemoveComponentFromEntity(ECSComponentType componentID, ECSEntityID entityID)
		{
			ECSSignature signatureToRemove;
			signatureToRemove.set((ui32)componentID);
			signatureToRemove.flip();

			ECSSignature oldSignature = m_entitySignatures.Get(entityID);
			ECSSignature newSignature = oldSignature & signatureToRemove;
			m_entitySignatures.Set(entityID, newSignature);
			m_entityComponents.Get(entityID).Set(componentID, nullptr);

			for (ui32 i = 0; i < m_systems.Size(); ++i)
			{
				m_systems[i]->OnEntityModified(oldSignature);
			}
		}

		void ECSManager::RemoveSystem(ECSSystem* system)
		{
			m_systems.RemoveFast(system);
		}
	}
}