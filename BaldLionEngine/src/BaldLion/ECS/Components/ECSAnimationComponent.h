#pragma once
#include "BaldLion/ECS/ECSComponent.h"
#include "BaldLion/Core/StringId.h"
#include "BaldLion/Core/Time.h"
#include "BaldLion/Animation/AnimationManager.h"

namespace BaldLion {

	using namespace Animation;

	namespace ECS
	{
		class ECSAnimationComponent : public ECSComponent
		{
		public:

			ECSAnimationComponent(ui32 animatorID, ui32 initAnimationId) :
				ECSComponent(ECSComponentType::Animation), animatorID(animatorID), currentAnimationID(initAnimationId), currentAnimationTime(0.0f), currentTransitionTime(0.0f), timer(Time::RequestNewTimer())
			{
				animatorParameters = HashTable<StringId, AnimatorParameter>(MemoryManager::GetAllocatorType(this), AnimationManager::GetAnimator(animatorID)->GetAllParameters());
			}

		public:

			HashTable<StringId, AnimatorParameter> animatorParameters;

			ui32 animatorID;
			ui32 currentAnimationID;
			Timer* timer;

			float currentAnimationTime;
			float currentTransitionTime;
		};
	}
}
