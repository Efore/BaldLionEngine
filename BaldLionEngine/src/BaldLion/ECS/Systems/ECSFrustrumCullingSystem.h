#pragma once
#include "BaldLion/ECS/ECSSystem.h"

namespace BaldLion {

	namespace ECS {

		class ECSFrustrumCullingSystem : public ECSSystem
		{
		public:
			ECSFrustrumCullingSystem(const char* systemName, const ECSSignature& signature, class ECSManager* ecsManager) :
				ECSSystem(systemName, signature, ecsManager, false, false, JobManagement::Job::JobType::Rendering) {}
			
			virtual void OnStart() override {};
			virtual void OnUpdate(float deltaTime) override;
			virtual void UpdateComponents(ECSEntityID entityID, ECSComponentLookUp* componentLookUp, float deltaTime) override;
			virtual void OnStop() override {};

		};

	}
}

