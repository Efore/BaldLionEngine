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
				ECSComponent(ECSComponentType::Animation), animatorID(animatorID), currentAnimationID(initAnimationId), currentAnimationTime(0.0f), currentTransitionTime(0.0f)
			{
				Time::RequestNewTimer(timer);
				animatorParameters = HashTable<StringId, AnimatorParameter>(MemoryManager::GetAllocatorType(this), AnimationManager::GetAnimator(animatorID)->GetAllParameters());
			}

		public:

			float GetParameterFloat(const StringId parameterName)
			{
				AnimatorParameter parameter;
				if (animatorParameters.TryGet(parameterName, parameter))
				{
					return parameter.Value.floating;
				}

				return -1;
			}

			void SetParameterFloat(const StringId parameterName, float value)
			{
				AnimatorParameter parameter;
				if (animatorParameters.TryGet(parameterName, parameter))
				{
					parameter.Value.floating = value;
				}
			}

			ui32 GetParameterInt(const StringId parameterName)
			{
				AnimatorParameter parameter;
				if (animatorParameters.TryGet(parameterName, parameter))
				{
					return parameter.Value.integer;
				}
				return -1;
			}

			void SetParameterInt(const StringId parameterName, ui32 value)
			{
				AnimatorParameter parameter;
				if (animatorParameters.TryGet(parameterName, parameter))
				{
					parameter.Value.integer = value;
				}
			}

			bool GetParameterBool(const StringId parameterName)
			{
				AnimatorParameter parameter;
				if (animatorParameters.TryGet(parameterName, parameter))
				{
					return parameter.Value.boolean;
				}

				return false;
			}

			void SetParameterBool(const StringId parameterName, bool value)
			{
				AnimatorParameter parameter;
				if (animatorParameters.TryGet(parameterName, parameter))
				{
					parameter.Value.boolean = value;
				}
			}

		public:

			HashTable<StringId, AnimatorParameter> animatorParameters;

			ui32 animatorID;
			ui32 currentAnimationID;
			Timer timer;

			float currentAnimationTime;
			float currentTransitionTime;
		};
	}
}
