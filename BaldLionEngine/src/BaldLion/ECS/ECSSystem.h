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
			ECSSystem(const char* systemName, const ECSSignature& signature, class ECSManager* ecsManager, bool blockSystemsLoop);
			virtual ~ECSSystem();

			virtual void OnStart() = 0;
			void OnUpdate(TimeStep timeStep);
			virtual void OnStop() = 0;

			virtual void UpdateOperation(TimeStep timeStep, ECSComponentLookUp* componentLookUp) = 0;

		protected:

			DynamicArray<ECSComponentLookUp*> m_componentLookUps;
			ECSSignature m_signature;
			class ECSManager* m_ecsManager;
			StringId m_systemName;
			bool m_blockSystemsLoop;			
		};
	}
}