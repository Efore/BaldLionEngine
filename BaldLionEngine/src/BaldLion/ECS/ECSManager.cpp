#include "blpch.h"
#include "ECSManager.h"
#include "BaldLion/ECS/ECSComponentsInclude.h"
#include <glm/gtx/matrix_decompose.hpp>

namespace BaldLion 
{
	namespace ECS {		

		ui32 ECSManager::m_entityIDProvider = 1;
		ui32 ECSManager::m_componentIDProvider = 1;

		ECSManager::ECSManager()
		{
			m_entityComponents = HashTable<ECSEntityID, ECSComponentLookUp>(AllocationType::FreeList_ECS,100);
			m_entitySignatures = HashTable<ECSEntityID, ECSSignature>(AllocationType::FreeList_ECS, 100);
			m_entitiyMap = HashTable<ECSEntityID, ECSEntity*>(AllocationType::FreeList_ECS, 100);

			m_entities = DynamicArray<ECSEntity>(AllocationType::FreeList_ECS,100);			
			m_systems = DynamicArray<ECSSystem*>(AllocationType::FreeList_ECS,100);

			m_componentsPool = DynamicArray<void*>(AllocationType::FreeList_ECS, (ui32)ECSComponentType::Count);

			m_cachedEntityHierarchy = DynamicArray<ECSTransformHierarchyEntry>(AllocationType::FreeList_ECS, 30);

			//Pools initialization in ecscomponenttype order: 
			//	1-Transform
			//	2-ProjectionCamera
			//	3-Mesh
			//	4-Skeleton
			//	5-DirectionalLight
			//	6-Animation
			//	7-PhysicsBody

			m_transformComponentPool = DynamicArray<ECSTransformComponent>(AllocationType::FreeList_ECS, 1000);
			m_componentsPool.EmplaceBack(&m_transformComponentPool);

			m_projectionCameraComponentPool = DynamicArray<ECSProjectionCameraComponent>(AllocationType::FreeList_ECS, 10);
			m_componentsPool.EmplaceBack(&m_projectionCameraComponentPool);

			m_meshComponentPool = DynamicArray<ECSMeshComponent>(AllocationType::FreeList_ECS, 100);
			m_componentsPool.EmplaceBack(&m_meshComponentPool);

			m_skeletonComponentPool = DynamicArray<ECSSkeletonComponent>(AllocationType::FreeList_ECS, 40);
			m_componentsPool.EmplaceBack(&m_skeletonComponentPool);			

			m_directionalLightComponentPool = DynamicArray<ECSDirectionalLightComponent>(AllocationType::FreeList_ECS, 10);
			m_componentsPool.EmplaceBack(&m_directionalLightComponentPool);

			m_animationComponentPool = DynamicArray<ECSAnimationComponent>(AllocationType::FreeList_ECS, 40);
			m_componentsPool.EmplaceBack(&m_animationComponentPool);

			m_physicsBodyComponentPool = DynamicArray<ECSPhysicsBodyComponent>(AllocationType::FreeList_ECS, 40);
			m_componentsPool.EmplaceBack(&m_physicsBodyComponentPool);

			m_entityIDProvider = 1;
			m_componentIDProvider = 1;
		}

		ECSManager::~ECSManager()
		{
			m_entityComponents.Delete();
			m_entitySignatures.Delete();
			m_entities.Delete();
			m_cachedEntityHierarchy.Delete();

			BL_DYNAMICARRAY_FOR(i, m_systems, 0)			
			{
				MemoryManager::Delete(m_systems[i]);
			}

			m_systems.Delete();			

			CleanComponentPool<ECSTransformComponent>(ECSComponentType::Transform);
			CleanComponentPool<ECSProjectionCameraComponent>(ECSComponentType::ProjectionCamera);
			CleanComponentPool<ECSMeshComponent>(ECSComponentType::Mesh);
			CleanComponentPool<ECSSkeletonComponent>(ECSComponentType::Skeleton);
			CleanComponentPool<ECSDirectionalLightComponent>(ECSComponentType::DirectionalLight);
			CleanComponentPool<ECSAnimationComponent>(ECSComponentType::Animation);
			CleanComponentPool<ECSPhysicsBodyComponent>(ECSComponentType::PhysicsBody);

			m_componentsPool.Delete();
		}

		ECSEntityID ECSManager::AddEntity(const char* entityName)
		{
			ECSEntityID entityID = ECSManager::GetNextEntityID();

			m_entities.EmplaceBack(entityName, entityID);
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

		void ECSManager::RemoveEntity(ECSEntityID entityID)
		{
			DynamicArray<ECSEntityID> entitiesIDToRemove(AllocationType::Linear_Frame, m_entities.Size());
			
			FillEntityHierarchyList(entityID, entitiesIDToRemove);

			BL_DYNAMICARRAY_FOR(i, entitiesIDToRemove, 0)
			{
				InternalRemoveEntity(entitiesIDToRemove[i]);
			}

			GenerateCachedHierarchy();
		}
		
		
		void ECSManager::InternalRemoveEntity(ECSEntityID entityID)
		{
			ECSSignature oldSignature = m_entitySignatures.Get(entityID);

			BL_DYNAMICARRAY_FOR(i, m_systems, 0)
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

		void ECSManager::RemoveComponentFromPool(ECSComponentType componentType, const ECSComponent* componentToRemove)
		{
			DynamicArray<ECSComponent>* componentPool = (DynamicArray<ECSComponent>*)m_componentsPool[(ui32)componentType];
			componentPool->RemoveFast(*componentToRemove);

			m_componentsPool[(ui32)componentType] = componentPool;
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

		void ECSManager::SetHierarchy(ECSEntityID entityID, ECSEntityID parentID)
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

		void ECSManager::GenerateCachedHierarchy()
		{
			m_cachedEntityHierarchy.Clear();

			BL_HASHTABLE_FOR(m_entitiyMap, it)
			{
				ECSEntity* entity = it.GetValue();

				if (entity->GetChildrenIDs().Size() > 0 && entity->GetParentID() == 0)
				{
					ECS::ECSTransformComponent* transformComponent = (ECS::ECSTransformComponent*)m_entityComponents.Get(entity->GetEntityID())[(ui32)ECSComponentType::Transform];

					ui32 currentCachedHierarchySize = m_cachedEntityHierarchy.Size();

					m_cachedEntityHierarchy.PushBack({ transformComponent, glm::mat4(1.0f), -1 });

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
			ECS::ECSTransformComponent* transformComponent = (ECS::ECSTransformComponent*)m_entityComponents.Get(entity->GetEntityID())[(ui32)ECSComponentType::Transform];

			glm::mat4 parentWorldMatrix = m_cachedEntityHierarchy[parentIndex].transformComponent->GetTransformMatrix();			

			ui32 currentCachedHierarchySize = m_cachedEntityHierarchy.Size();
			m_cachedEntityHierarchy.PushBack({ transformComponent, parentWorldMatrix, parentIndex });

			BL_DYNAMICARRAY_FOREACH(entity->GetChildrenIDs())
			{
				if (entity->GetChildrenIDs().Size() > 0)
				{
					FillCachedHierarchy(entity->GetChildrenIDs()[i], currentCachedHierarchySize);
				}
			}
		}

		void ECSManager::StartSystems()
		{
			BL_DYNAMICARRAY_FOR(i, m_systems, 0)
			{
				m_systems[i]->OnStart();
			}
		}

		void ECSManager::StopSystems()
		{
			BL_DYNAMICARRAY_FOR(i, m_systems, 0)
			{
				m_systems[i]->OnStop();
			}
		}

		void ECSManager::AddSystem(ECSSystem* system)
		{
			m_systems.PushBack(system);
		}

		void ECSManager::RemoveSystem(ECSSystem* system)
		{
			m_systems.RemoveFast(system);
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

		void ECSManager::UpdateHierarchyTransforms()
		{
			BL_DYNAMICARRAY_FOREACH(m_cachedEntityHierarchy)
			{
				if (m_cachedEntityHierarchy[i].parentIndex > -1) 
				{
					ECS::ECSTransformComponent* transformComponent = m_cachedEntityHierarchy[i].transformComponent;
					const glm::mat4 relativeToParentTransform = glm::inverse(m_cachedEntityHierarchy[i].parentWorldTransform) * transformComponent->GetTransformMatrix();
					const glm::mat4 parentWorldTransform = m_cachedEntityHierarchy[m_cachedEntityHierarchy[i].parentIndex].transformComponent->GetTransformMatrix();
					const glm::mat4 newWorldTransform = parentWorldTransform * relativeToParentTransform;

					glm::vec3 deltaPosition;
					glm::quat deltaRotationQuat;
					glm::vec3 deltaScale;
					glm::vec3 skew;
					glm::vec4 perspective;
					glm::decompose(newWorldTransform, deltaScale, deltaRotationQuat, deltaPosition, skew, perspective);

					transformComponent->position = deltaPosition;
					transformComponent->rotation = glm::eulerAngles(deltaRotationQuat);
					transformComponent->scale = deltaScale;					

					m_cachedEntityHierarchy[i].parentWorldTransform = parentWorldTransform;
				}
			}
		}


		//---------------------------------------------------------------------------------------------------
	}
}