#include "blpch.h"
#include "ECSManager.h"
#include "BaldLion/ECS/ECSComponentsInclude.h"
#include "BaldLion/ECS/SingletonSystems/CameraSystem.h"
#include "BaldLion/Utils/MathUtils.h"

namespace BaldLion 
{
	namespace ECS {		

		ui32 ECSManager::s_entityIDProvider = 1;
		ui32 ECSManager::s_componentIDProvider = 1;

		ECSManager::ECSManager()
		{
			m_entityComponents = HashTable<ECSEntityID, ECSComponentLookUp>(AllocationType::FreeList_ECS,1024);
			m_entitySignatures = HashTable<ECSEntityID, ECSSignature>(AllocationType::FreeList_ECS, 1024);
			m_entitiyMap = HashTable<ECSEntityID, ECSEntity*>(AllocationType::FreeList_ECS, 1024);

			memset(m_systems, 0, sizeof(ECSSystem*) * (ui32)ECSSystemType::Count);

			m_entities = DynamicArray<ECSEntity>(AllocationType::FreeList_ECS,1024);		
			m_cachedEntityHierarchy = DynamicArray<ECSTransformHierarchyEntry>(AllocationType::FreeList_ECS, 1024);

			m_transformComponentPool = DynamicArray<ECSTransformComponent>(AllocationType::FreeList_ECS, 1024);
			m_componentsPool[(ui32)ECSComponentType::Transform] = &m_transformComponentPool;

			m_projectionCameraComponentPool = DynamicArray<ECSProjectionCameraComponent>(AllocationType::FreeList_ECS, 2);
			m_componentsPool[(ui32)ECSComponentType::ProjectionCamera] = &m_projectionCameraComponentPool;
			
			m_meshComponentPool = DynamicArray<ECSMeshComponent>(AllocationType::FreeList_ECS, 1024);
			m_componentsPool[(ui32)ECSComponentType::Mesh] = &m_meshComponentPool;

			m_skeletonComponentPool = DynamicArray<ECSSkeletonComponent>(AllocationType::FreeList_ECS, 32);
			m_componentsPool[(ui32)ECSComponentType::Skeleton] = &m_skeletonComponentPool;

			m_directionalLightComponentPool = DynamicArray<ECSDirectionalLightComponent>(AllocationType::FreeList_ECS, 4);
			m_componentsPool[(ui32)ECSComponentType::DirectionalLight] = &m_directionalLightComponentPool;

			m_animationComponentPool = DynamicArray<ECSAnimationComponent>(AllocationType::FreeList_ECS, 32);
			m_componentsPool[(ui32)ECSComponentType::Animation] = &m_animationComponentPool;

			m_physicsBodyComponentPool = DynamicArray<ECSPhysicsBodyComponent>(AllocationType::FreeList_ECS, 32);
			m_componentsPool[(ui32)ECSComponentType::PhysicsBody] = &m_physicsBodyComponentPool;

			m_navMeshAgentComponentPool = DynamicArray<ECSNavMeshAgentComponent>(AllocationType::FreeList_ECS, 32);
			m_componentsPool[(ui32)ECSComponentType::NavMeshAgent] = &m_navMeshAgentComponentPool;

			m_locomotionComponentPool = DynamicArray<ECSLocomotionComponent>(AllocationType::FreeList_ECS, 32);
			m_componentsPool[(ui32)ECSComponentType::Locomotion] = &m_locomotionComponentPool;

			m_cameraFollowComponentPool = DynamicArray<ECSCameraFollowComponent>(AllocationType::FreeList_ECS, 2);
			m_componentsPool[(ui32)ECSComponentType::CameraFollow] = &m_cameraFollowComponentPool;

			m_playerControllerComponentPool = DynamicArray<ECSPlayerControllerComponent>(AllocationType::FreeList_ECS, 2);
			m_componentsPool[(ui32)ECSComponentType::PlayerController] = &m_playerControllerComponentPool;

			m_htnAgentComponentPool = DynamicArray<ECSHTNAgentComponent>(AllocationType::FreeList_ECS, 32);
			m_componentsPool[(ui32)ECSComponentType::HTNAgent] = &m_htnAgentComponentPool;
		}

		ECSManager::~ECSManager()
		{
			m_entityComponents.Delete();
			m_entitySignatures.Delete();
			m_entities.Delete();
			m_cachedEntityHierarchy.Delete();

			for(ui32 i = 0; i < (ui32)ECSSystemType::Count; ++i)
			{
				MemoryManager::Delete(m_systems[i]);
			}	

			ClearComponentPool<ECSTransformComponent>(ECSComponentType::Transform);
			ClearComponentPool<ECSProjectionCameraComponent>(ECSComponentType::ProjectionCamera);
			ClearComponentPool<ECSMeshComponent>(ECSComponentType::Mesh);
			ClearComponentPool<ECSSkeletonComponent>(ECSComponentType::Skeleton);
			ClearComponentPool<ECSDirectionalLightComponent>(ECSComponentType::DirectionalLight);
			ClearComponentPool<ECSAnimationComponent>(ECSComponentType::Animation);
			ClearComponentPool<ECSPhysicsBodyComponent>(ECSComponentType::PhysicsBody);
			ClearComponentPool<ECSNavMeshAgentComponent>(ECSComponentType::NavMeshAgent);
			ClearComponentPool<ECSLocomotionComponent>(ECSComponentType::Locomotion);
			ClearComponentPool<ECSCameraFollowComponent>(ECSComponentType::CameraFollow);
			ClearComponentPool<ECSPlayerControllerComponent>(ECSComponentType::PlayerController);
			ClearComponentPool<ECSHTNAgentComponent>(ECSComponentType::HTNAgent);

		}

		ECSEntityID ECSManager::AddEntity(const char* entityName)
		{
			ECSEntityID entityID = ECSManager::GetNextEntityID();

			AddEntity(entityName, entityID);

			return entityID;
		}

		void ECSManager::AddEntity(const char* entityName, ECSEntityID entityID)
		{
			m_entities.EmplaceBack(entityName, entityID);
			m_entitiyMap.Emplace(entityID, &m_entities[m_entities.Size() - 1]);

			ECSSignature newSignature;
			newSignature.reset();

			m_entitySignatures.Emplace(entityID, std::move(newSignature));

			ECSComponentLookUp newComponentLookUp;
			m_entityComponents.Emplace(entityID, std::move(newComponentLookUp));

			ECSSignature signature = m_entitySignatures.Get(entityID);

			for (ui32 i = 0; i < (ui32)ECSSystemType::Count; ++i)
			{
				m_systems[i]->OnEntityModified(signature);
			}
		}

		void ECSManager::RemoveEntity(ECSEntityID entityID)
		{
			DynamicArray<ECSEntityID> entitiesIDToRemove(AllocationType::Linear_Frame, m_entities.Size());
			
			FillEntityHierarchyList(entityID, entitiesIDToRemove);

			BL_DYNAMICARRAY_FOR(i, entitiesIDToRemove, 0)
			{
				InternalRemoveEntity(entitiesIDToRemove[i]);
			}

			GenerateCachedHierarchyRoot();
		}		
		
		void ECSManager::InternalRemoveEntity(ECSEntityID entityID)
		{
			ECSSignature oldSignature = m_entitySignatures.Get(entityID);

			for (ui32 i = 0; i < (ui32)ECSSystemType::Count; ++i)
			{
				m_systems[i]->OnEntityModified(oldSignature);
			}

			i32 indexToRemove = -1;
			BL_DYNAMICARRAY_FOR(i, m_entities, 0)
			{
				if (m_entities[i].GetEntityID() == entityID) 
				{
					indexToRemove = i;
				}

				if (m_entities[i].GetChildrenIDs().Contains(entityID))
				{
					m_entities[i].GetChildrenIDs().Remove(entityID);
				}
			}

			m_entities.RemoveAt(indexToRemove);

			ECSComponentLookUp components;

			if (m_entityComponents.TryGet(entityID, components)) 
			{
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

		void ECSManager::AddComponentToEntity(ECSEntityID entityID, ECSComponent* component)
		{
			const ECSComponentType componentType = component->GetComponentType();
			component->SetComponentID(ECSManager::GetNextComponentID());
			component->SetOwnerEntityID(entityID);

			ECSSignature componentSignature;
			componentSignature.set((ui32)componentType);

			m_entitySignatures.Get(entityID) |= componentSignature;
			m_entityComponents.Get(entityID).Set(componentType, component);

			ECSSignature signature = m_entitySignatures.Get(entityID);

			for (ui32 i = 0; i < (ui32)ECSSystemType::Count; ++i)
			{
				m_systems[i]->OnEntityModified(signature);
			}
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

			for (ui32 i = 0; i < (ui32)ECSSystemType::Count; ++i)
			{
				m_systems[i]->OnEntityModified(oldSignature);
			}
		}

		void ECSManager::RemoveComponentFromPool(ECSComponentType componentType, const ECSComponent* componentToRemove)
		{
			switch(componentType)
			{
			case ECSComponentType::Transform:
				InternalRemoveComponentFromPool<ECSTransformComponent>(componentType, (ECSTransformComponent*)componentToRemove);
				break;
			case ECSComponentType::ProjectionCamera:
				InternalRemoveComponentFromPool<ECSProjectionCameraComponent>(componentType, (ECSProjectionCameraComponent*)componentToRemove);
				break;
			case ECSComponentType::Mesh:
				InternalRemoveComponentFromPool<ECSMeshComponent>(componentType, (ECSMeshComponent*)componentToRemove);
				break;
			case ECSComponentType::Skeleton:
				InternalRemoveComponentFromPool<ECSSkeletonComponent>(componentType, (ECSSkeletonComponent*)componentToRemove);
				break;
			case ECSComponentType::DirectionalLight:
				InternalRemoveComponentFromPool<ECSDirectionalLightComponent>(componentType, (ECSDirectionalLightComponent*)componentToRemove);
				break;
			case ECSComponentType::Animation:
				InternalRemoveComponentFromPool<ECSAnimationComponent>(componentType, (ECSAnimationComponent*)componentToRemove);
				break;
			case ECSComponentType::PhysicsBody:
				InternalRemoveComponentFromPool<ECSPhysicsBodyComponent>(componentType, (ECSPhysicsBodyComponent*)componentToRemove);
				break;
			case ECSComponentType::NavMeshAgent:
				InternalRemoveComponentFromPool<ECSNavMeshAgentComponent>(componentType, (ECSNavMeshAgentComponent*)componentToRemove);
				break;
			case ECSComponentType::Locomotion:
				InternalRemoveComponentFromPool<ECSLocomotionComponent>(componentType, (ECSLocomotionComponent*)componentToRemove);
				break;
			case ECSComponentType::CameraFollow:
				InternalRemoveComponentFromPool<ECSCameraFollowComponent>(componentType, (ECSCameraFollowComponent*)componentToRemove);
				break;
			case ECSComponentType::PlayerController:
				InternalRemoveComponentFromPool<ECSPlayerControllerComponent>(componentType, (ECSPlayerControllerComponent*)componentToRemove);
				break;
			case ECSComponentType::HTNAgent:
				InternalRemoveComponentFromPool<ECSHTNAgentComponent>(componentType, (ECSHTNAgentComponent*)componentToRemove);
				break;
			case ECSComponentType::Count:
				break;
			default:
				break;
			}
		}

		void ECSManager::FillEntityHierarchyList(ECSEntityID rootID, DynamicArray<ECSEntityID>& entitiesIDlist)
		{
			entitiesIDlist.PushBack(rootID);
			ECSEntity* root = m_entitiyMap.Get(rootID);

			BL_DYNAMICARRAY_FOR(i, root->GetChildrenIDs(), 0)
			{
				FillEntityHierarchyList(root->GetChildrenIDs()[i], entitiesIDlist);
			}
		}

		void ECSManager::SetEntityParent(ECSEntityID entityID, ECSEntityID parentID)
		{
			ECSEntity* entity = m_entitiyMap.Get(entityID);

			if (entity->GetParentID() > 0)
			{
				ECSEntity* oldParent = m_entitiyMap.Get(entity->GetParentID());
				oldParent->GetChildrenIDs().Remove(entityID);
			}

			if (parentID > 0)
			{
				ECSEntity* parentEntity = m_entitiyMap.Get(parentID);
				parentEntity->GetChildrenIDs().EmplaceBack(entityID);
			}

			entity->SetParentID(parentID);
		}

		void ECSManager::GenerateCachedHierarchyRoot()
		{
			m_cachedEntityHierarchy.Clear();

			BL_HASHTABLE_FOR(m_entitiyMap, it)
			{
				ECSEntity* entity = it.GetValue();

				if (entity->GetChildrenIDs().Size() > 0 && entity->GetParentID() == 0)
				{
					ECS::ECSTransformComponent* transformComponent = m_entityComponents.Get(entity->GetEntityID()).Write<ECS::ECSTransformComponent>(ECSComponentType::Transform);

					ui32 currentCachedHierarchySize = m_cachedEntityHierarchy.Size();

					m_cachedEntityHierarchy.PushBack({transformComponent, MathUtils::Vector3Zero, MathUtils::Vector3Zero, MathUtils::Vector3Unit, entity->GetEntityID() , -1, false });

					BL_DYNAMICARRAY_FOREACH(it.GetValue()->GetChildrenIDs())
					{
						FillCachedHierarchy(it.GetValue()->GetChildrenIDs()[i], currentCachedHierarchySize);
					}
				}
			}
		}
		
		void ECSManager::FillCachedHierarchy(ECSEntityID childEntityID, i32 parentIndex)
		{					
			ECSEntity* entity = m_entitiyMap.Get(childEntityID);
			ECS::ECSTransformComponent* transformComponent = m_entityComponents.Get(entity->GetEntityID()).Write<ECS::ECSTransformComponent>(ECSComponentType::Transform);
		
			ui32 currentCachedHierarchySize = m_cachedEntityHierarchy.Size();
			m_cachedEntityHierarchy.PushBack({ transformComponent,m_cachedEntityHierarchy[parentIndex].transformComponent->position, m_cachedEntityHierarchy[parentIndex].transformComponent->rotation, m_cachedEntityHierarchy[parentIndex].transformComponent->scale, childEntityID, parentIndex, false});

			BL_DYNAMICARRAY_FOREACH(entity->GetChildrenIDs())
			{
				if (entity->GetChildrenIDs().Size() > 0)
				{
					FillCachedHierarchy(entity->GetChildrenIDs()[i], currentCachedHierarchySize);
				}
			}
		}

		void ECSManager::UpdateHierarchyTransforms()
		{
			BL_DYNAMICARRAY_FOREACH(m_cachedEntityHierarchy)
			{
				if (m_cachedEntityHierarchy[i].parentIndex > -1 && m_cachedEntityHierarchy[m_cachedEntityHierarchy[i].parentIndex].transformHasChanged)
				{
					ECS::ECSTransformComponent* transformComponent = m_cachedEntityHierarchy[i].transformComponent;
				
					const glm::vec3 relativePosition = transformComponent->position - m_cachedEntityHierarchy[i].previousParentPosition;
					const glm::vec3 relativeRotation = transformComponent->rotation - m_cachedEntityHierarchy[i].previousParentRotation;
					const glm::vec3 relativeScale = transformComponent->scale - m_cachedEntityHierarchy[i].previousParentScale;

					m_cachedEntityHierarchy[i].previousParentPosition = m_cachedEntityHierarchy[m_cachedEntityHierarchy[i].parentIndex].transformComponent->position;
					m_cachedEntityHierarchy[i].previousParentRotation = m_cachedEntityHierarchy[m_cachedEntityHierarchy[i].parentIndex].transformComponent->rotation;
					m_cachedEntityHierarchy[i].previousParentScale = m_cachedEntityHierarchy[m_cachedEntityHierarchy[i].parentIndex].transformComponent->scale;
	
					transformComponent->position = m_cachedEntityHierarchy[i].previousParentPosition + relativePosition;
					transformComponent->rotation = m_cachedEntityHierarchy[i].previousParentRotation + relativeRotation;
					transformComponent->scale = m_cachedEntityHierarchy[i].previousParentScale + relativeScale;

					m_cachedEntityHierarchy[i].transformHasChanged = true;
				}
			}

			//Restoring hasChanged values
			BL_DYNAMICARRAY_FOREACH(m_cachedEntityHierarchy)
			{
				m_cachedEntityHierarchy[i].transformHasChanged = false;
			}
		}

		void ECSManager::MarkEntityTransformAsChangedInHierarchy(ECSEntityID entityID) 
		{
			BL_DYNAMICARRAY_FOREACH(m_cachedEntityHierarchy)
			{
				if (m_cachedEntityHierarchy[i].entityID == entityID)
				{
					m_cachedEntityHierarchy[i].transformHasChanged = true;
					break;
				}
			}
		}

		void ECSManager::StartSystems()
		{
			for (ui32 i = 0; i < (ui32)ECSSystemType::Count; ++i)
			{
				m_systems[i]->OnStart();
			}
		}

		void ECSManager::StopSystems()
		{
			for (ui32 i = 0; i < (ui32)ECSSystemType::Count; ++i)
			{
				m_systems[i]->OnStop();
			}
		}

		void ECSManager::AddSystem(ECSSystem* system)
		{
			m_systems[(ui32)system->GetSystemType()] = system;
		}

		ECSSystem* ECSManager::GetSystem(ECSSystemType systemType) const
		{
			return m_systems[(ui32)systemType];
		}

		void ECSManager::FrameStart()
		{
			for (ui32 i = 0; i < (ui32)ECSSystemType::Count; ++i)
			{
				m_systems[i]->OnFrameStart();
			}
		}

		void ECSManager::UpdateSystems(float deltaTime)
		{
			for (ui32 i = 0; i < (ui32)ECSSystemType::Count; ++i)
			{
				m_systems[i]->OnUpdate(deltaTime);
			}
		}

		void ECSManager::FrameEnd()
		{
			for (ui32 i = 0; i < (ui32)ECSSystemType::Count; ++i)
			{
				m_systems[i]->OnFrameEnd();
			}

			UpdateHierarchyTransforms();
		}

		


		//---------------------------------------------------------------------------------------------------
	}
}