#pragma once
#include "ECSUtils.h"
#include "ECSComponentLookUp.h"
#include "BaldLion/Core/Containers/DynamicArray.h"
#include "BaldLion/Core/Threading/TaskScheduler.h"

namespace BaldLion
{
	namespace ECS {		
		
		class ECSSystem {

		public:
			ECSSystem(const char* systemName, ECSSystemType systemType, const ECSSignature& signature, class ECSManager* ecsManager, bool parallelize);
			virtual ~ECSSystem();

			virtual void OnStart() = 0;
			virtual void OnUpdate(float deltaTime);
			virtual void OnStop() = 0;
			virtual void OnFrameStart();
			virtual void OnFrameEnd();

			virtual void UpdateComponents(ECSEntityID entityID, ECSComponentLookUp* componentLookUp, float deltaTime) = 0;

			void OnEntityModified(ECSSignature entitySignature);	

			ECSSystemType GetSystemType() const { return m_systemType; }

			void SetParallelDependencies(ui32 count, ...);

			const Threading::Task& GetParallelTask() const { return m_parallelTask; }

		protected:

			StringId m_systemName;
			ECSSystemType m_systemType;
			ECSSignature m_signature;			
			DynamicArray<ECSEntityID> m_entityIDs;
			DynamicArray<ECSComponentLookUp*> m_componentLookUps;

			const Threading::Task* m_parallelDependencies[(ui32)ECSSystemType::Count];
			ui32 m_parallelDependenciesCount = 0;

			class ECSManager* m_ecsManager;		

			bool m_refreshComponentLookUps;
			bool m_firstFrame = true;

			bool m_parallelize;
			Threading::Task m_parallelTask;
		};

		#define BL_GENERATE_SYSTEM(SystemName, SystemClass, SystemType, EcsManager, ...)	\
				{	\
				const ECS::ECSSignature systemSignature = GenerateSignature(BL_NUMARGS(__VA_ARGS__),__VA_ARGS__);	\
				SystemClass* ecsSystem = MemoryManager::New<SystemClass>(SystemName, AllocationType::FreeList_ECS, SystemName, SystemType, systemSignature, EcsManager);	\
				EcsManager->AddSystem(ecsSystem);	\
				}

		#define BL_SET_SYSTEM_DEPENDENCIES(SystemType, EcsManager, ...)	\
				{	\
				ECS::ECSSystem* ecsSystem = EcsManager->GetSystem(SystemType);	\
				ecsSystem->SetParallelDependencies(BL_NUMARGS(__VA_ARGS__),__VA_ARGS__);	\
				}

	}
}
