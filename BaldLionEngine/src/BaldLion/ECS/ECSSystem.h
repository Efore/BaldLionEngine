#pragma once
#include "ECSUtils.h"
#include "ECSComponentLookUp.h"
#include "BaldLion/Core/TimeStep.h"
#include "BaldLion/Core/Containers/DynamicArray.h"

namespace BaldLion
{
	namespace ECS {
		
		
		class ECSSystem {

		public:
			ECSSystem(const char* systemName, const ECSSignature& signature, class ECSManager* ecsManager, bool parallelize, bool blockSystemsLoop);
			virtual ~ECSSystem();

			virtual void OnStart() = 0;
			virtual void OnUpdate(TimeStep timeStep);
			virtual void OnStop() = 0;
			virtual void OnFrameStart();
			virtual void OnFrameEnd();

			virtual void UpdateOperation(TimeStep timeStep, ECSComponentLookUp* componentLookUp) = 0;

			void OnEntityModified(ECSSignature entitySignature);					

		protected:

			ECSSignature m_signature;
			DynamicArray<ECSComponentLookUp*> m_componentLookUps;

			class ECSManager* m_ecsManager;
			StringId m_systemName;

			bool m_waitForUpdatesOperationsToFinish;			
			bool m_parallelize;

			bool m_refreshComponentLookUps;
		};
	}
}
