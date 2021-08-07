#pragma once
#pragma once
#include "BaldLion/ECS/ECSComponent.h"
#include <glm/glm.hpp>

namespace BaldLion {

	namespace ECS
	{
		class ECSProjectionCameraComponent : public ECSComponent
		{
		public:

			ECSProjectionCameraComponent(float width, float height, float nearPlane, float farPlane, float cameraMovementSpeed, float cameraRotationSpeed) :
				ECSComponent(ECSComponentID::ProjectionCamera),
				width(width),
				height(height),
				nearPlane(nearPlane),
				farPlane(farPlane),
				cameraMovementSpeed(cameraMovementSpeed),
				cameraRotationSpeed(cameraRotationSpeed)
			{ 
			}

		public:
			glm::mat4 viewProjectionMatrix;

			float width;
			float height;

			float nearPlane;
			float farPlane;

			float prevX;
			float prevY;

			float cameraYaw;
			float cameraPitch;

			float cameraRotationSpeed;
			float cameraMovementSpeed;

		};
	}
}