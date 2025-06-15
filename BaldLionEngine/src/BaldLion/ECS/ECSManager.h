#pragma once

#include <type_traits>
#include "ECSComponent.h"
#include "ECSSystem.h"
#include "ECSUtils.h"
#include "ECSEntity.h"
#include "Components/ECSTransformComponent.h"
#include "BaldLion/Core/Containers/HashMap.h"
#include "BaldLion/Core/Containers/HashTable.h"

namespace BaldLion {

	namespace ECS {		

		//---------------------------------------------------------------------------------------------------

		struct ECSTransformHierarchyEntry
		{
			glm::mat4 parentWorldTransform;
			ECS::ECSTransformComponent* transformComponent;
			ECSEntityID entityID;
			i32 parentIndex;
			bool transformHasChanged;
		};

		class ECSManager {
			
		public:

			ECSManager();
			ECSManager(const ECSManager&) = delete;

			ECSManager& operator=(const ECSManager&) = delete;		

			~ECSManager();

			//Entities
			ECSEntityID AddEntity(const char* entityName);
			void AddEntity(const char* entityName, ECSEntityID entityID);
			void RemoveEntity(ECSEntityID entityID);			

			//Components		
			template <typename T>
			const DynamicArray<T>* GetComponentPool(ECSComponentType componentType) const;

			template <typename T, typename... Args >
			T* CreateComponent(ECSComponentType componentID, Args&&... args);
			void AddComponentToEntity(ECSEntityID entityID, ECSComponent* component);
			void RemoveComponentFromEntity(ECSComponentType componentID, ECSEntityID entityID);

			//Systems
			void AddSystem(ECSSystem* system);		
			ECSSystem* GetSystem(ECSSystemType systemType) const;			

			//Hierarchy
			void SetEntityParent(ECSEntityID entityID, ECSEntityID parentID);
			void GenerateCachedHierarchyRoot();
			void MarkEntityTransformAsChangedInHierarchy(ECSEntityID entityID);

			//Main loop
			void StartSystems();
			void FrameStart();
			void UpdateSystems(float deltaTime);
			void FrameEnd();
			void StopSystems();

			HashTable<ECSEntityID, ECSComponentLookUp>& GetEntityComponents() { return m_entityComponents; }
			const HashTable<ECSEntityID, ECSComponentLookUp>& GetEntityComponents() const { return m_entityComponents; }

			HashTable<ECSEntityID, ECSSignature>& GetEntitySignatures() { return m_entitySignatures; }
			const HashTable<ECSEntityID, ECSSignature>& GetEntitySignatures() const { return m_entitySignatures; }

			HashTable<ECSEntityID, ECSEntity*>& GetEntityMap() { return m_entitiyMap; }
			const HashTable<ECSEntityID, ECSEntity*>& GetEntityMap() const { return m_entitiyMap; }

			DynamicArray<ECSEntity>& GetEntities() { return m_entities; }
			const DynamicArray<ECSEntity>& GetEntities() const { return m_entities; }

			const DynamicArray<ECSTransformHierarchyEntry>& GetCachedEntityHierarchy() const { return m_cachedEntityHierarchy; }

			static ui32 GetNextEntityID() { return s_entityIDProvider++; }
			static ui32 GetNextComponentID() { return s_componentIDProvider++; }

			static ui32 GetLatestEntityID() { return s_entityIDProvider; }
			static ui32 GetLatestComponentID() { return s_componentIDProvider; }

			static void SetEntityIDProvider(ui32 value) { s_entityIDProvider = value; }
			static void SetComponentIDProvider(ui32 value) { s_componentIDProvider = value; }

		private:

			template<typename T>
			void CleanComponentPool(ECSComponentType componentType);	

			void RemoveComponentFromPool(ECSComponentType componentType, const ECSComponent* componentToRemove);

			template<typename T>
			void InternalRemoveComponentFromPool(ECSComponentType componentType, const T* componentToRemove);

			void FillEntityHierarchyList(ECSEntityID rootID, DynamicArray<ECSEntityID>& entitiesIDlist);
			void InternalRemoveEntity(ECSEntityID entityID);

			void FillCachedHierarchy(ECSEntityID childEntityID, i32 parentIndex);
			void UpdateHierarchyTransforms();

		private:

			HashTable<ECSEntityID, ECSComponentLookUp> m_entityComponents;
			HashTable<ECSEntityID, ECSSignature> m_entitySignatures;
			HashTable<ECSEntityID, ECSEntity*> m_entitiyMap;

			DynamicArray<ECSEntity> m_entities;
			ECSSystem* m_systems[(ui32)ECSSystemType::Count];

			DynamicArray<ECSTransformHierarchyEntry> m_cachedEntityHierarchy;

			//Pools
			void* m_componentsPool[(ui32)ECSComponentType::Count];
			DynamicArray<class ECSTransformComponent> m_transformComponentPool;
			DynamicArray<class ECSProjectionCameraComponent> m_projectionCameraComponentPool;			
			DynamicArray<class ECSDirectionalLightComponent> m_directionalLightComponentPool;
			DynamicArray<class ECSMeshComponent> m_meshComponentPool;
			DynamicArray<class ECSAnimationComponent> m_animationComponentPool;
			DynamicArray<class ECSSkeletonComponent> m_skeletonComponentPool;			
			DynamicArray<class ECSPhysicsBodyComponent> m_physicsBodyComponentPool;
			DynamicArray<class ECSNavMeshAgentComponent> m_navMeshAgentComponentPool;
			DynamicArray<class ECSLocomotionComponent> m_locomotionComponentPool;
			DynamicArray<class ECSCameraFollowComponent> m_cameraFollowComponentPool;
			DynamicArray<class ECSPlayerControllerComponent> m_playerControllerComponentPool;

			static ui32 s_entityIDProvider;
			static ui32 s_componentIDProvider;
		};

		template<typename T>
		void BaldLion::ECS::ECSManager::InternalRemoveComponentFromPool(ECSComponentType componentType, const T* componentToRemove)
		{
			static_assert(std::is_base_of<ECSComponent, T>::value, "T must inherit from Component");
			DynamicArray<T>* componentPool = static_cast<DynamicArray<T>*>(m_componentsPool[(ui32)componentType]);
			
			i32 componentIndexInPool = componentPool->FindIndex(*componentToRemove);
			ECSComponentLookUp* componentLookUp = nullptr;

			//Since we will use remove at fast, we need to account for the swapped element and make sure that the componentLookUp that is 
			//referenzing it is corrected
			if (componentPool->Size() > 1 && componentIndexInPool < (componentPool->Size() - 1))
			{
				T* switchingComponent = &(componentPool->Back());
				BL_HASHTABLE_FOR(m_entityComponents, iterator)
				{
					if (iterator.GetValue()[(ui32)componentType] == switchingComponent)
					{
						componentLookUp = &iterator.GetValue();
						break;
					}
				}
			}

			componentPool->RemoveAtFast(componentIndexInPool);

			if (componentLookUp != nullptr)
			{
				componentLookUp->Set(componentType, static_cast<ECSComponent*>(&(*componentPool)[componentIndexInPool]));
			}
		}

		template <typename T>
		const DynamicArray<T>*
			BaldLion::ECS::ECSManager::GetComponentPool(ECSComponentType componentType) const
		{
			static_assert(std::is_base_of<ECSComponent, T>::value, "T must inherit from Component");
			return static_cast<const DynamicArray<T>*>(m_componentsPool[(ui32)componentType]);			
		}

		template<typename T>
		void BaldLion::ECS::ECSManager::CleanComponentPool(ECSComponentType componentType)
		{
			static_assert(std::is_base_of<ECSComponent, T>::value, "T must inherit from Component");
			((DynamicArray<T>*)m_componentsPool[(ui32)componentType])->Delete();
		}

		template <typename T, typename...Args >
		T* BaldLion::ECS::ECSManager::CreateComponent(ECSComponentType componentType, Args&&... args)
		{
			static_assert(std::is_base_of<ECSComponent, T>::value, "T must inherit from Component");
			DynamicArray<T>* componentPool = static_cast<DynamicArray<T>*>(m_componentsPool[(ui32)componentType]);
			return componentPool->EmplaceBack(std::forward<Args>(args)...);
		}
	}
}
