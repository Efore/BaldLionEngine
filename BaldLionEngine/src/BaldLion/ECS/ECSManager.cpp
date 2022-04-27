#include "blpch.h"
#include "ECSManager.h"
#include "BaldLion/ECS/ECSComponentsInclude.h"

namespace BaldLion {

	namespace ECS {

		//---------------------------------------------------------------------------------------------------

		ui32 ECSManager::m_entityIDProvider = 1;
		ui32 ECSManager::m_componentIDProvider = 1;

		ECSManager::ECSManager()
		{
			m_entityComponents = HashMap<ECSEntityID, ECSComponentLookUp>(AllocationType::FreeList_ECS,100);
			m_entitySignatures = HashMap<ECSEntityID, ECSSignature>(AllocationType::FreeList_ECS, 100);
			m_entitiyMap = HashMap<ECSEntityID, ECSEntity*>(AllocationType::FreeList_ECS, 100);

			m_entities = DynamicArray<ECSEntity>(AllocationType::FreeList_ECS,100);			
			m_systems = DynamicArray<ECSSystem*>(AllocationType::FreeList_ECS,100);

			m_componentsPool = DynamicArray<void*>(AllocationType::FreeList_ECS, (ui32)ECSComponentType::Count);

			//Pools initialization
			m_transformComponentPool = DynamicArray<ECSTransformComponent>(AllocationType::FreeList_ECS, 1000);
			m_componentsPool.EmplaceBack(&m_transformComponentPool);

			m_projectionCameraComponentPool = DynamicArray<ECSProjectionCameraComponent>(AllocationType::FreeList_ECS, 10);
			m_componentsPool.EmplaceBack(&m_projectionCameraComponentPool);

			m_directionalLightComponentPool = DynamicArray<ECSDirectionalLightComponent>(AllocationType::FreeList_ECS, 10);
			m_componentsPool.EmplaceBack(&m_directionalLightComponentPool);

			m_meshComponentPool = DynamicArray<ECSMeshComponent>(AllocationType::FreeList_ECS, 100);
			m_componentsPool.EmplaceBack(&m_meshComponentPool);

			m_animationComponentPool = DynamicArray<ECSAnimationComponent>(AllocationType::FreeList_ECS, 40);
			m_componentsPool.EmplaceBack(&m_animationComponentPool);

			m_skeletonComponentPool = DynamicArray<ECSSkeletonComponent>(AllocationType::FreeList_ECS, 40);
			m_componentsPool.EmplaceBack(&m_skeletonComponentPool);

			m_hierarchyComponentPool = DynamicArray<ECSHierarchyComponent>(AllocationType::FreeList_ECS, 40);
			m_componentsPool.EmplaceBack(&m_hierarchyComponentPool);

			m_entityIDProvider = 1;
			m_componentIDProvider = 1;
		}

		ECSManager::~ECSManager()
		{
			m_entityComponents.Delete();
			m_entitySignatures.Delete();
			m_entities.Delete();

			BL_DYNAMICARRAY_FOR(i, m_systems, 0)			
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
			ECSEntityID entityID = ECSManager::GetNextEntityID();

			ECSEntity newEntity(entityName, entityID);

			m_entities.PushBack(std::move(newEntity));
			m_entitiyMap.Emplace(entityID, &m_entities[m_entities.Size() - 1]);

			ECSSignature newSignature;
			newSignature.reset();

			m_entitySignatures.Emplace(entityID, std::move(newSignature));

			ECSComponentLookUp newComponentLookUp;
			m_entityComponents.Emplace(entityID, std::move(newComponentLookUp));				

			ECSSignature signature = m_entitySignatures.Get(entityID);

			BL_DYNAMICARRAY_FOR(i, m_systems, 0)			
			{
				m_systems[i]->OnEntityModified(signature);
			}

			return entityID;
		}

		void ECSManager::AddComponentToEntity(ECSEntityID entityID, ECSComponent* component)
		{
			const ECSComponentType componentType = component->GetComponentType();
			component->SetComponentID(ECSManager::GetNextComponentID());

			ECSSignature componentSignature;
			componentSignature.set((ui32)componentType);

			m_entitySignatures.Get(entityID) |= componentSignature;
			m_entityComponents.Get(entityID).Set(componentType, component);

			ECSSignature signature = m_entitySignatures.Get(entityID);

			BL_DYNAMICARRAY_FOR(i, m_systems, 0)
			{
				m_systems[i]->OnEntityModified(signature);
			}
		}

		void ECSManager::AddSystem(ECSSystem* system)
		{
			m_systems.PushBack(system);
		}

		void ECSManager::StartSystems()
		{
			BL_DYNAMICARRAY_FOR(i, m_systems, 0)
			{
				m_systems[i]->OnStart();
			}
		}

		void ECSManager::FrameStart()
		{
			BL_DYNAMICARRAY_FOR(i, m_systems, 0)
			{
				m_systems[i]->OnFrameStart();
			}
		}

		void ECSManager::UpdateSystems()
		{
			BL_DYNAMICARRAY_FOR(i, m_systems, 0)
			{
				m_systems[i]->OnUpdate();
			}
		}

		void ECSManager::FrameEnd()
		{
			BL_DYNAMICARRAY_FOR(i, m_systems, 0)
			{
				m_systems[i]->OnFrameEnd();
			}
		}

		void ECSManager::StopSystems()
		{
			BL_DYNAMICARRAY_FOR(i, m_systems, 0)
			{
				m_systems[i]->OnStop();
			}
		}

		void ECSManager::RemoveEntity(ECSEntityID entityID)
		{
			ECSSignature oldSignature = m_entitySignatures.Get(entityID);
			BL_DYNAMICARRAY_FOR(i, m_systems, 0)
			{
				m_systems[i]->OnEntityModified(oldSignature);
			}

			BL_DYNAMICARRAY_FOR(i, m_entities, 0)
			{				
				if (m_entities[i].GetEntityID() == entityID) {
					m_entities.RemoveAt(i);
					break;
				}				
			}
			ECSComponentLookUp components;

			if (m_entityComponents.TryGet(entityID, components)) {

				for (ui32 i = 0; i < (ui32)ECSComponentType::Count; ++i)
				{
					const ECSComponent* componentToRemove = components[i];
					if (componentToRemove != nullptr)
					{
						RemoveComponentFromPool((ECSComponentType)i, componentToRemove);
						components.Set((ECSComponentType)i, nullptr);
					}
				}
			}
			
			m_entitiyMap.Remove(entityID);
			m_entityComponents.Remove(entityID);
			m_entitySignatures.Remove(entityID);
		}

		void ECSManager::RemoveComponentFromEntity(ECSComponentType componentType, ECSEntityID entityID)
		{
			ECSSignature signatureToRemove;
			signatureToRemove.set((ui32)componentType);
			signatureToRemove.flip();

			ECSSignature oldSignature = m_entitySignatures.Get(entityID);
			ECSSignature newSignature = oldSignature & signatureToRemove;
			m_entitySignatures.Set(entityID, newSignature);

			ECSComponent* componentToRemove = m_entityComponents.Get(entityID)[(ui32)componentType];
		
			RemoveComponentFromPool(componentType, componentToRemove);

			m_entityComponents.Get(entityID).Set(componentType, nullptr);
			
			BL_DYNAMICARRAY_FOR(i, m_systems, 0)
			{
				m_systems[i]->OnEntityModified(oldSignature);
			}
		}

		void ECSManager::RemoveSystem(ECSSystem* system)
		{
			m_systems.RemoveFast(system);
		}

		void ECSManager::RemoveComponentFromPool(ECSComponentType componentType, const ECSComponent* componentToRemove)
		{
			DynamicArray<ECSComponent>* componentPool = (DynamicArray<ECSComponent>*)m_componentsPool[(ui32)componentType];
			componentPool->Remove(*componentToRemove);

			m_componentsPool[(ui32)componentType] = componentPool;
		}

	}
}