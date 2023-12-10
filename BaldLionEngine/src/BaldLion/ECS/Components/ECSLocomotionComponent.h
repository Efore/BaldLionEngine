#pragma once

#include "BaldLion/ECS/ECSComponent.h"
#include <glm/glm.hpp>

namespace BaldLion
{
	namespace ECS
	{
		class ECSLocomotionComponent : public ECSComponent {

		public:

			ECSLocomotionComponent(float rotationSpeed, float maxMovementSpeed, float accelerationSpeed) :
				ECSComponent(ECSComponentType::Locomotion), rotationSpeed(rotationSpeed), currentMovementSpeed(0.0f), maxMovementSpeed(maxMovementSpeed), maxAcceleration(accelerationSpeed), currentVelocity(0.0f), desiredVelocity(0.0f)
			{				
			}

			~ECSLocomotionComponent()
			{				
			}

		public:						
			float rotationSpeed;
			float currentMovementSpeed;
			float maxMovementSpeed;
			float maxAcceleration;

			glm::vec3 currentVelocity;
			glm::vec3 desiredVelocity;
		};
	}
}

