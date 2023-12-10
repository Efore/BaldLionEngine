#pragma once
#include "BaldLion/ECS/ECSComponent.h"

namespace BaldLion {

	namespace ECS
	{
		class ECSPlayerControllerComponent : public ECSComponent
		{
		public:

			ECSPlayerControllerComponent() :
				ECSComponent(ECSComponentType::PlayerController)
			{
			}
		};
	}
}
