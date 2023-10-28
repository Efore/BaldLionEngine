#include "blpch.h"
#include "ECSSystem.h"
#include "ECSManager.h"
#include "BaldLion/Core/Containers/HashMap.h"
#include "BaldLion/Core/JobManagement/JobManager.h"

namespace BaldLion {

	namespace ECS {

		ECSSystem::ECSSystem(const char* systemName, const ECSSignature& signature, ECSManager* ecsManager, bool parallelize, bool waitForUpdateOperationsToFinish) :
			m_systemName(BL_STRING_TO_STRINGID(systemName)), 
			m_signature(signature), 
			m_ecsManager(ecsManager),
			m_parallelize(parallelize),
			m_waitForUpdatesOperationsToFinish(waitForUpdateOperationsToFinish),
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

			BL_DYNAMICARRAY_FOR(i, m_componentLookUps, 0)
			{
				ECSComponentLookUp* componentLookUp = m_componentLookUps[i];
				const ECSEntityID entityID = m_entityIDs[i];

				if (m_parallelize)
				{
					const std::string systemOperation = std::to_string(m_systemName) + std::to_string(i);

					JobManagement::Job systemUpdateJob(systemOperation.c_str(), JobManagement::Job::JobType::ECS);

					systemUpdateJob.Task = [this, entityID, componentLookUp, deltaTime] {

						this->UpdateComponents(entityID, componentLookUp, deltaTime);
					};

					JobManagement::JobManager::QueueJob(systemUpdateJob);
				}
				else 
				{
					this->UpdateComponents(entityID, componentLookUp, deltaTime);
				}
			}

			if (m_parallelize && m_waitForUpdatesOperationsToFinish)
			{
				JobManagement::JobManager::WaitForJobs(1 << JobManagement::Job::JobType::ECS);
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