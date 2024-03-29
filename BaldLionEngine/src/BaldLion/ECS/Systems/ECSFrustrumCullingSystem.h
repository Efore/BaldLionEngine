#pragma once
#include "BaldLion/ECS/ECSSystem.h"

namespace BaldLion {

	namespace ECS {

		class ECSFrustrumCullingSystem : public ECSSystem
		{
		public:
			ECSFrustrumCullingSystem(const char* systemName, ECSSystemType systemType, const ECSSignature& signature, class ECSManager* ecsManager) :
				ECSSystem(systemName,systemType, signature, ecsManager, true) {}
			
			virtual void OnStart() override {};
			virtual void OnUpdate(float deltaTime) override;
			virtual void UpdateComponents(ECSEntityID entityID, ECSComponentLookUp* componentLookUp, float deltaTime) override;
			virtual void OnStop() override {};

			virtual void OnEntityModified(ECSSignature entitySignature) override;
		};

	}
}

