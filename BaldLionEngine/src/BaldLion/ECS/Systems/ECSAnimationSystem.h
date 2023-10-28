#pragma once
#include "BaldLion/ECS/ECSSystem.h"
#include "BaldLion/Animation/Animator.h"


namespace BaldLion {

	using namespace Animation;

	namespace ECS {

		class ECSAnimationSystem : public ECSSystem
		{
		public:
			ECSAnimationSystem(const char* systemName, const ECSSignature& signature, class ECSManager* ecsManager) :
				ECSSystem(systemName, signature, ecsManager, true, true) {}

			virtual void OnStart() override {}
			virtual void UpdateComponents(ECSEntityID entityID, ECSComponentLookUp* componentLookUp, float deltaTime) override;
			virtual void OnStop() override {}
	
		private: 
			void CalculateInterpolatedTransforms(float currentAnimationTime, float currentTransitionTime, const AnimationClip* animation, const Animator* animator, const AnimatorTransition* transition, JointTransform* result) const;
		};

	}
}

