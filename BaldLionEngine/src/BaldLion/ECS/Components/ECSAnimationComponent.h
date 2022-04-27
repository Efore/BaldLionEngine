#pragma once
#include "BaldLion/ECS/ECSComponent.h"
#include "BaldLion/Core/StringId.h"
#include "BaldLion/Core/Time.h"

namespace BaldLion {

	namespace ECS
	{
		class ECSAnimationComponent : public ECSComponent
		{
		public:

			ECSAnimationComponent(ui32 animatorID, ui32 initAnimationId) :
				ECSComponent(ECSComponentType::Animation), animatorID(animatorID), currentAnimationID(initAnimationId), currentAnimationTime(0.0f), currentTransitionTime(0.0f), timer(Time::RequestNewTimer())
			{
			}

		public:
			ui32 animatorID;
			ui32 currentAnimationID;
			Timer* timer;

			float currentAnimationTime;
			float currentTransitionTime;
		};
	}
}
