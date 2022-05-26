#pragma once
#include "BaldLion/ECS/ECSSystem.h"
#include "BaldLion/Animation/AnimationClip.h"


namespace BaldLion {

	using namespace Animation;

	namespace ECS {

		class ECSAnimationSystem : public ECSSystem
		{
		public:
			ECSAnimationSystem(const char* systemName, const ECSSignature& signature, class ECSManager* ecsManager) :
				ECSSystem(systemName, signature, ecsManager, true, true) {}

			virtual void OnStart() override {}
			virtual void UpdateOperation(ECSComponentLookUp* componentLookUp) override;
			virtual void OnStop() override {}
	
		};

	}
}

