#include "blpch.h"
#include "ECSManager.h"
#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/ECS/Components/ECSProjectionCameraComponent.h"

namespace BaldLion {

	namespace ECS {

		//---------------------------------------------------------------------------------------------------

		ECSManager::ECSManager()
		{
			m_entityComponents = HashTable<ECSEntityID, ECSComponentLookUp>(AllocationType::FreeList_Main,100);
			m_entitySignatures = HashTable<ECSEntityID, ECSSignature>(AllocationType::FreeList_Main, 100);
			
			m_systems = DynamicArray<ECSSystem*>(AllocationType::FreeList_Main,100);

			m_componentsPool = HashTable<ECSComponentID, void*>(AllocationType::FreeList_Main, 100);

			//Pools initialization
			m_transformComponentPool = DynamicArray<ECSTransformComponent>(AllocationType::FreeList_Main, 1000);
			m_componentsPool.Emplace(ECSComponentID::Transform, std::move(&m_transformComponentPool));

			m_projectionCameraComponentPool = DynamicArray<ECSProjectionCameraComponent>(AllocationType::FreeList_Main, 1000);
			m_componentsPool.Emplace(ECSComponentID::ProjectionCamera, std::move(&m_projectionCameraComponentPool));
		}

		ECSManager::~ECSManager()
		{
			m_entityComponents.Clear();
			m_entitySignatures.Clear();

			for (ui32 i = 0; i < m_systems.Size(); ++i)
			{
				MemoryManager::Delete(m_systems[i]);
			}

			m_systems.Clear();			

			CleanComponentPool< ECSTransformComponent>(ECSComponentID::Transform);
			CleanComponentPool< ECSProjectionCameraComponent>(ECSComponentID::ProjectionCamera);

			m_componentsPool.Clear();
		}

		void ECSManager::AddEntity(ECSEntityID entityID)
		{
			BL_ASSERT(!m_entitySignatures.Contains(entityID), "Entity already exists");

			ECSSignature newSignature;
			newSignature.reset();

			m_entitySignatures.Emplace(entityID, std::move(newSignature));

			ECSComponentLookUp newComponentLookUp;
			m_entityComponents.Emplace(entityID, std::move(newComponentLookUp));
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

		void ECSManager::StopSystems()
		{
			for (ui32 i = 0; i < m_systems.Size(); ++i)
			{
				m_systems[i]->OnStop();
			}
		}

		void ECSManager::RemoveEntity(ECSEntityID entityID)
		{
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
		}

		void ECSManager::RemoveSystem(ECSSystem* system)
		{
			m_systems.RemoveFast(system);
		}
	}
}