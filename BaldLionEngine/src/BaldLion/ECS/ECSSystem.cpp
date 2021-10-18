#include "blpch.h"
#include "ECSSystem.h"
#include "ECSManager.h"
#include "BaldLion/Core/Containers/HashTable.h"
#include "BaldLion/Core/JobManagement/JobManager.h"

namespace BaldLion {

	namespace ECS {

		ECSSystem::ECSSystem(const char* systemName, const ECSSignature& signature, ECSManager* ecsManager, bool parallelize, bool waitForUpdateOperationsToFinish) :
			m_systemName(STRING_TO_STRINGID(systemName)), 
			m_signature(signature), 
			m_ecsManager(ecsManager),
			m_parallelize(parallelize),
			m_waitForUpdatesOperationsToFinish(waitForUpdateOperationsToFinish)
		{
			m_componentLookUps = DynamicArray<ECSComponentLookUp*>(AllocationType::FreeList_ECS, 100);
			
			for (HashTable<ECSEntityID, ECSSignature>::Iterator iterator = m_ecsManager->GetEntitySignatures().Begin(); iterator != m_ecsManager->GetEntitySignatures().End(); ++iterator)
			{
				if ((iterator.GetValue() & signature) == signature)
				{
					ECSComponentLookUp* componentLookUp = &(m_ecsManager->GetEntityComponents().Get(iterator.GetKey()));
					m_componentLookUps.PushBack(componentLookUp); 
				}
			}
		}

		void ECSSystem::OnUpdate(TimeStep timeStep)
		{	
			for (ui32 i = 0; i < m_componentLookUps.Size(); ++i)
			{
				ECSComponentLookUp* componentLookUp = m_componentLookUps[i];

				if (m_parallelize)
				{
					const std::string systemOperation = std::to_string(m_systemName) + std::to_string(i);

					JobManagement::Job systemUpdateJob(systemOperation.c_str());

					systemUpdateJob.Task = [this, timeStep, componentLookUp] {

						this->UpdateOperation(timeStep, componentLookUp);
					};

					JobManagement::JobManager::QueueJob(systemUpdateJob);
				}
				else 
				{
					this->UpdateOperation(timeStep, componentLookUp);
				}
			}

			if (m_parallelize && m_waitForUpdatesOperationsToFinish)
			{
				JobManagement::JobManager::WaitForJobs();
			}
		}

		ECSSystem::~ECSSystem()
		{
			m_componentLookUps.Delete();
		}
	}
}