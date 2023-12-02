#pragma once
#include "ECSUtils.h"
#include "ECSComponentLookUp.h"
#include "BaldLion/Core/Containers/DynamicArray.h"
#include "BaldLion/Core/JobManagement/JobManager.h"



namespace BaldLion
{
	using namespace JobManagement;

	namespace ECS {		
		
		class ECSSystem {

		public:
			ECSSystem(const char* systemName, const ECSSignature& signature, class ECSManager* ecsManager, bool parallelize, bool blockSystemsLoop, Job::JobType jobType = Job::JobType::ECS);
			virtual ~ECSSystem();

			virtual void OnStart() = 0;
			virtual void OnUpdate(float deltaTime);
			virtual void OnStop() = 0;
			virtual void OnFrameStart();
			virtual void OnFrameEnd();

			virtual void UpdateComponents(ECSEntityID entityID, ECSComponentLookUp* componentLookUp, float deltaTime) = 0;

			void OnEntityModified(ECSSignature entitySignature);					

		protected:

			ECSSignature m_signature;			
			DynamicArray<ECSEntityID> m_entityIDs;
			DynamicArray<ECSComponentLookUp*> m_componentLookUps;

			class ECSManager* m_ecsManager;
			StringId m_systemName;

			bool m_waitForUpdatesOperationsToFinish;			
			bool m_parallelize;

			bool m_refreshComponentLookUps;

			Job::JobType m_jobType;
		};

		#define BL_GENERATE_SYSTEM(SystemName, SystemType, ecsManager, ...)	\
				{	\
				const ECS::ECSSignature systemSignature = GenerateSignature(BL_NUMARGS(__VA_ARGS__),__VA_ARGS__);	\
				SystemType* ecsSystem = MemoryManager::New<SystemType>(SystemName, AllocationType::FreeList_ECS, SystemName, systemSignature, ecsManager);	\
				ecsManager->AddSystem(ecsSystem);	\
				}

	}
}
