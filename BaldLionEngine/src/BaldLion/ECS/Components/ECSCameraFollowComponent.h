#pragma once
#include "BaldLion/ECS/ECSComponent.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


namespace BaldLion {

	namespace ECS
	{
		class ECSCameraFollowComponent : public ECSComponent
		{
		public:

			ECSCameraFollowComponent(ECSEntityID followedEntity, const glm::vec2& offsetXY, float offsetZ, float rotationSpeed) :
				ECSComponent(ECSComponentType::CameraFollow),
				followedEntityID(followedEntity),
				offsetXY(offsetXY),
				offsetZ(offsetZ),
				rotationSpeed(rotationSpeed),
				prevInputValue(0.0f)
			{ 
			}

		public:
			ECSEntityID followedEntityID;

			glm::vec3 prevEntityFollowedPosition;
			glm::mat4 cameraTargetTransform;

			glm::vec2 offsetXY;
			float offsetZ;
			float rotationSpeed;
			float interpolator = 1.0f;			
			
			glm::vec2 prevInputValue;
			float cameraYaw = 0.0f;
			float cameraPitch = 0.0f;			
		};

	}
}
