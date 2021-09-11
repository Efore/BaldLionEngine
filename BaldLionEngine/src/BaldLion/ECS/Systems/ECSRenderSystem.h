#pragma once
#include "BaldLion/ECS/ECSSystem.h"

namespace BaldLion {

	namespace ECS {

		class ECSRenderSystem : public ECSSystem
		{
		public:
			ECSRenderSystem(const char* systemName, const ECSSignature& signature, class ECSManager* ecsManager) :
				ECSSystem(systemName, signature, ecsManager, false, true) {}
			
			virtual void OnUpdate(TimeStep timeStep) override;
			virtual void UpdateOperation(TimeStep timeStep, ECSComponentLookUp* componentLookUp) override;			

		private:

		};

	}
}

