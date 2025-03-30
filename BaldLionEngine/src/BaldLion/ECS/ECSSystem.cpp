#include "blpch.h"
#include "ECSSystem.h"
#include "ECSManager.h"
#include "BaldLion/Core/Containers/HashMap.h"

namespace BaldLion {

	namespace ECS {

		ECSSystem::ECSSystem(const char* systemName, ECSSystemType systemType, const ECSSignature& signature, ECSManager* ecsManager, bool parallelize) :
			m_systemName(BL_STRING_TO_STRINGID(systemName)), 
			m_systemType(systemType),
			m_signature(signature), 
			m_ecsManager(ecsManager),
			m_parallelize(parallelize),
			m_refreshComponentLookUps(false)
		{
			m_componentLookUps = DynamicArray<ECSComponentLookUp*>(AllocationType::FreeList_ECS, 96);
			m_entityIDs = DynamicArray<ECSEntityID>(AllocationType::FreeList_ECS, 96);
			
			BL_HASHMAP_FOR(m_ecsManager->GetEntitySignatures(), iterator)
			{
				if ((iterator.GetValue() & m_signature) == m_signature)
				{
					ECSComponentLookUp* componentLookUp = &(m_ecsManager->GetEntityComponents().Get(iterator.GetKey()));
					m_componentLookUps.PushBack(componentLookUp); 
					m_entityIDs.PushBack(iterator.GetKey());
				}
			}

			memset(m_parallelDependencies, 0, sizeof(Threading::TaskID*) * (ui32)ECSSystemType::Count);
		}

		ECSSystem::~ECSSystem()
		{
			m_parallelTask.Wait();
			m_componentLookUps.Delete();
		}

		void ECSSystem::OnUpdate(float deltaTime)
		{			
			if (m_componentLookUps.Size() == 0)
				return;

			if (m_parallelDependenciesCount > 0)
			{
				for (ui32 i = 0; i < m_parallelDependenciesCount; ++i)
				{
					m_parallelDependencies[i]->Wait();
				}
			}

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
			m_firstFrame = false;
		}

		void ECSSystem::OnEntityModified(ECSSignature entitySignature)
		{
			if ((entitySignature & m_signature) == m_signature) {
				m_refreshComponentLookUps = true;
			}			
		}

		void ECSSystem::SetParallelDependencies(ui32 numArgs, ...)
		{			
			std::va_list argsComponents;
			va_start(argsComponents, numArgs);

			ui32 count = 0;
			for (ui32 i = 0; i < numArgs; ++i)
			{
				ECSSystemType type = va_arg(argsComponents, ECSSystemType);
				ECSSystem* system = m_ecsManager->GetSystem(type);
				if (system != nullptr)
				{
					m_parallelDependencies[i] = &system->GetParallelTask();
					count++;
				}
			}

			m_parallelDependenciesCount = count;
		}
	}
}