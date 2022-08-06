#pragma once
#include "BaldLion/ECS/ECSSystem.h"

namespace BaldLion {

	namespace ECS {

		class ECSPhysicsSystem : public ECSSystem
		{
		public:
			ECSPhysicsSystem(const char* systemName, const ECSSignature& signature, class ECSManager* ecsManager);
			virtual ~ECSPhysicsSystem();

			virtual void OnStart() override {};
			virtual void OnUpdate() override;
			virtual void UpdateOperation(ECSComponentLookUp* componentLookUp) override;
			virtual void OnStop() override {};

		};

	}
}

