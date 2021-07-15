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
			ECSSystem(ECSSignature signature, class ECSManager* ecsManager);
			virtual ~ECSSystem();

			virtual void OnStart() = 0;
			virtual void OnUpdate(TimeStep timeStep) = 0;
			virtual void OnDestroy() = 0;

		protected:
			DynamicArray<ECSComponentLookUp*> m_componentLookUps;
			ECSSignature m_signature;
			class ECSManager* m_ecsManager;
		};
	}
}
