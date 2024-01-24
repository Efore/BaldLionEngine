#pragma once
#include "BaldLion/ECS/ECSSystem.h"
#include "BaldLion/Animation/Animator.h"


namespace BaldLion {

	using namespace Animation;

	namespace ECS {

		class ECSAnimationSystem : public ECSSystem
		{
		public:
			ECSAnimationSystem(const char* systemName, ECSSystemType systemType, const ECSSignature& signature, class ECSManager* ecsManager) :
				ECSSystem(systemName, systemType, signature, ecsManager, true) {}

			virtual void OnStart() override {}
			virtual void UpdateComponents(ECSEntityID entityID, ECSComponentLookUp* componentLookUp, float deltaTime) override;
			virtual void OnStop() override {}

#if BL_PROFILE_ACTIVE
			virtual void OnUpdate(float deltaTime) override
			{
				BL_PROFILE_FUNCTION();
				ECSSystem::OnUpdate(deltaTime);
			}
#endif
	
		private: 
			void CalculateInterpolatedTransforms(float currentAnimationTime, float currentTransitionTime, const AnimationClip* animation, const Animator* animator, const AnimatorTransition* transition, JointTransform* result) const;
		};

	}
}

