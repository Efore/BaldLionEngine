#pragma once
#include "BaldLion/ECS/ECSComponent.h"
#include "BaldLion/Core/StringId.h"

namespace BaldLion {

	namespace ECS
	{
		class ECSAnimationComponent : public ECSComponent
		{
		public:

			ECSAnimationComponent(StringId animatorID, StringId initAnimationId) :
				ECSComponent(ECSComponentType::Animation), animatorID(animatorID), currentAnimationID(initAnimationId), currentAnimationProgress(0.0f)
			{
			}

		public:
			StringId animatorID;
			StringId currentAnimationID;
			float currentAnimationProgress;
		};
	}
}
