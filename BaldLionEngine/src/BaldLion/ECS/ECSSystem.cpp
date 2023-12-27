#include "blpch.h"
#include "ECSSystem.h"
#include "ECSManager.h"
#include "BaldLion/Core/Containers/HashMap.h"

namespace BaldLion {

	namespace ECS {

		ECSSystem::ECSSystem(const char* systemName, const ECSSignature& signature, ECSManager* ecsManager, bool parallelize) :
			m_systemName(BL_STRING_TO_STRINGID(systemName)), 
			m_signature(signature), 
			m_ecsManager(ecsManager),
			m_parallelize(parallelize),
			m_refreshComponentLookUps(false)
		{

			m_componentLookUps = DynamicArray<ECSComponentLookUp*>(AllocationType::FreeList_ECS, 100);
			m_entityIDs = DynamicArray<ECSEntityID>(AllocationType::FreeList_ECS, 100);
			
			BL_HASHMAP_FOR(m_ecsManager->GetEntitySignatures(), iterator)
			{
				if ((iterator.GetValue() & m_signature) == m_signature)
				{
					ECSComponentLookUp* componentLookUp = &(m_ecsManager->GetEntityComponents().Get(iterator.GetKey()));
					m_componentLookUps.PushBack(componentLookUp); 
					m_entityIDs.PushBack(iterator.GetKey());
				}
			}
		}

		ECSSystem::~ECSSystem()
		{
			m_componentLookUps.Delete();
		}

		void ECSSystem::OnUpdate(float deltaTime)
		{	
			BL_PROFILE_FUNCTION();

			if (m_componentLookUps.Size() == 0)
				return;


			if (m_parallelize)
			{
				Threading::TaskScheduler::KickParallelTask(m_parallelTask, m_componentLookUps.Size(),
					[this,deltaTime](uint32_t firstIterationIndex, uint32_t lastIterationIndex)
					{
						for (uint32_t i = firstIterationIndex; i <= lastIterationIndex; ++i)
						{
							ECSComponentLookUp* componentLookUp = m_componentLookUps[i];
							const ECSEntityID entityID = m_entityIDs[i];

							this->UpdateComponents(entityID, componentLookUp, deltaTime);
						}
					});

				m_parallelTask.Wait();
			}
			else 
			{
				BL_DYNAMICARRAY_FOR(i, m_componentLookUps, 0)
				{
					ECSComponentLookUp* componentLookUp = m_componentLookUps[i];
					const ECSEntityID entityID = m_entityIDs[i];

					this->UpdateComponents(entityID, componentLookUp, deltaTime);
				}
			}
		}

		void ECSSystem::OnFrameStart()
		{
			BL_PROFILE_FUNCTION();

			if (m_refreshComponentLookUps) {

				m_componentLookUps.Clear();

				BL_HASHMAP_FOR(m_ecsManager->GetEntitySignatures(), iterator)
				{
					if ((iterator.GetValue() & m_signature) == m_signature)
					{
						ECSComponentLookUp* componentLookUp = &(m_ecsManager->GetEntityComponents().Get(iterator.GetKey()));
						m_componentLookUps.PushBack(componentLookUp);
						m_entityIDs.PushBack(iterator.GetKey());
					}
				}

				m_refreshComponentLookUps = false;
			}
		}

		void ECSSystem::OnFrameEnd()
		{
			BL_PROFILE_FUNCTION();
		}

		void ECSSystem::OnEntityModified(ECSSignature entitySignature)
		{
			if ((entitySignature & m_signature) == m_signature) {
				m_refreshComponentLookUps = true;
			}			
		}
	}
}