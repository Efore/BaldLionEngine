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

			ECSProjectionCameraComponent(float fov, float width, float height, float nearPlane, float farPlane, float cameraMovementSpeed, float cameraRotationSpeed) :
				ECSComponent(ECSComponentType::ProjectionCamera),
				fov(fov),
				width(width),
				height(height),
				nearPlane(nearPlane),
				farPlane(farPlane),
				cameraMovementSpeed(cameraMovementSpeed),
				cameraRotationSpeed(cameraRotationSpeed),
				prevX(0.0f),
				prevY(0.0f),
				cameraYaw(0.0f),
				cameraPitch(0.0f)
			{ 
			}
			
		public:
			glm::mat4 viewProjectionMatrix;

			float fov;

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