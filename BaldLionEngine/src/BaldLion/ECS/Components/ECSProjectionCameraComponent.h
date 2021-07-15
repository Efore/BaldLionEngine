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

			ECSProjectionCameraComponent(ECSComponentID componentID, const glm::vec3& position, float width, float height, float nearPlane, float farPlane) :
				ECSComponent(componentID),
				width(width),
				height(height),
				nearPlane(nearPlane),
				farPlane(farPlane)
			{ 
			}

		public:

			float width;
			float height;

			float nearPlane;
			float farPlane;

			float prevX;
			float prevY;

			float cameraPitchRotation;
			float cameraYawRotation;

			float cameraRotationSpeed;
			float cameraMovementSpeed;

		};
	}
}