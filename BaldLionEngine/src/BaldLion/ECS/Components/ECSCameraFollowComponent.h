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
			ECSEntityID followedEntityID = -1;

			glm::vec3 prevEntityFollowedPosition = glm::vec3(0.0f);
			glm::mat4 cameraTargetTransform = glm::mat4(1.0f);

			glm::vec2 offsetXY = glm::vec2(0.0f);
			float offsetZ = 0.0f;
			float rotationSpeed = 0.0f;
			float interpolator = 1.0f;			
			
			glm::vec2 prevInputValue = glm::vec2(0.0f);
			float cameraYaw = 0.0f;
			float cameraPitch = 0.0f;			
		};

	}
}
