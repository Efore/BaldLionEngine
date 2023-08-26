#pragma once

#include "BaldLion/ECS/ECSComponent.h"
#include <glm/glm.hpp>

namespace BaldLion
{
	namespace ECS
	{
		class ECSLocomotionComponent : public ECSComponent {

		public:

			ECSLocomotionComponent(float rotationSpeed) :
				ECSComponent(ECSComponentType::Locomotion), rotationSpeed(rotationSpeed), currentVelocity(0.0f), desiredVelocity(0.0f)
			{				
			}

			~ECSLocomotionComponent()
			{				
			}

		public:						
			float rotationSpeed;		
			glm::vec3 currentVelocity;
			glm::vec3 desiredVelocity;
		};
	}
}

