#pragma once
#include "BaldLion/ECS/ECSSystem.h"
#include "BaldLion/Animation/AnimationData.h"

namespace BaldLion {

	namespace ECS {

		class ECSAnimationSystem : public ECSSystem
		{
		public:
			ECSAnimationSystem(const char* systemName, const ECSSignature& signature, class ECSManager* ecsManager) :
				ECSSystem(systemName, signature, ecsManager, true, true) {}

			virtual void OnStart() override {};
			virtual void OnUpdate(TimeStep timeStep) override {};
			virtual void UpdateOperation(TimeStep timeStep, ECSComponentLookUp* componentLookUp) override;
			virtual void OnStop() override {};

		private:
			void CalculateInterpolatedTransforms(float progress, const AnimationData* animation, DynamicArray<JointTransform>& result);
		};

	}
}

