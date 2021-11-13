#pragma once

#include "ECSComponent.h"
#include "ECSSystem.h"
#include "ECSUtils.h"
#include "ECSEntity.h"
#include "BaldLion/Core/TimeStep.h"
#include "BaldLion/Core/Containers/HashMap.h"
#include "BaldLion/Core/Containers/HashTable.h"

namespace BaldLion {

	namespace ECS {		

		//---------------------------------------------------------------------------------------------------

		class ECSManager {
			
		public:

			ECSManager();
			ECSManager(const ECSManager&) = delete;

			ECSManager& operator=(const ECSManager&) = delete;		

			~ECSManager();

			//Add elements
			ECSEntityID AddEntity(const char* entityName);
			void AddComponentToEntity(ECSEntityID entityID, ECSComponent* component);
			void AddSystem(ECSSystem* system);			
			
			template <typename ECSComponentType, typename... Args >
			ECSComponentType* AddComponent(ECSComponentID componentID, Args&&... args);

			//Main loop
			void StartSystems();
			void UpdateSystems(TimeStep timeStep);
			void EndOfFrame();
			void StopSystems();

			//Remove elements
			void RemoveEntity(ECSEntityID entityID);			
			void RemoveComponentFromEntity(ECSComponentID componentID, ECSEntityID entityID);
			void RemoveSystem(ECSSystem* system);

			HashMap<ECSEntityID, ECSComponentLookUp>& GetEntityComponents() { return m_entityComponents; }
			const HashMap<ECSEntityID, ECSComponentLookUp>& GetEntityComponents() const { return m_entityComponents; }

			HashMap<ECSEntityID, ECSSignature>& GetEntitySignatures() { return m_entitySignatures; }
			const HashMap<ECSEntityID, ECSSignature>& GetEntitySignatures() const { return m_entitySignatures; }

			HashMap<ECSEntityID, ECSEntity*>& GetEntityMap() { return m_entitiyMap; }
			const HashMap<ECSEntityID, ECSEntity*>& GetEntityMap() const { return m_entitiyMap; }

			DynamicArray<ECSEntity>& GetEntities() { return m_entities; }
			const DynamicArray<ECSEntity>& GetEntities() const { return m_entities; }


		private:

			template<typename ECSComponentType>
			void CleanComponentPool(ECSComponentID componentID);			

		private:

			HashMap<ECSEntityID, ECSComponentLookUp> m_entityComponents;
			HashMap<ECSEntityID, ECSSignature> m_entitySignatures;

			HashMap<ECSEntityID, ECSEntity*> m_entitiyMap;

			DynamicArray<ECSEntity> m_entities;
			DynamicArray<ECSSystem*> m_systems;

			//Pools
			HashTable<ECSComponentID, void*> m_componentsPool;
			DynamicArray<class ECSTransformComponent> m_transformComponentPool;
			DynamicArray<class ECSProjectionCameraComponent> m_projectionCameraComponentPool;			
			DynamicArray<class ECSDirectionalLightComponent> m_directionalLightComponentPool;
			DynamicArray<class ECSMeshComponent> m_meshComponentPool;
			DynamicArray<class ECSAnimationComponent> m_animationComponentPool;
			DynamicArray<class ECSSkeletonComponent> m_skeletonComponentPool;
			DynamicArray<class ECSHierarchyComponent> m_hierarchyComponentPool;

			ui32 m_entityIDProvider;
		};

		template<typename ECSComponentType>
		void BaldLion::ECS::ECSManager::CleanComponentPool(ECSComponentID componentID)
		{
			((DynamicArray<ECSComponentType>*)m_componentsPool.Get(componentID))->Delete();
		}

		template <typename ECSComponentType, typename...Args >
		ECSComponentType* BaldLion::ECS::ECSManager::AddComponent(ECSComponentID componentID, Args&&... args)
		{
			DynamicArray<ECSComponentType>* componentPool = static_cast<DynamicArray<ECSComponentType>*>(m_componentsPool.Get(componentID));
			return componentPool->EmplaceBack(std::forward<Args>(args)...);
		}

	}
}
