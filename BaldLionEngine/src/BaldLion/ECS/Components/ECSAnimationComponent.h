#pragma once
#include "BaldLion/ECS/ECSComponent.h"
#include "BaldLion/Core/StringId.h"

namespace BaldLion {

	namespace ECS
	{
		class ECSAnimationComponent : public ECSComponent
		{
		public:

			ECSAnimationComponent() :
				ECSComponent(ECSComponentID::Animation)
			{
			}

		public:
			StringId animatorID;
			StringId currentAnimationID;
			float currentAnimationProgress;
		};
	}
}
