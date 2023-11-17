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
				rotationSpeed(rotationSpeed)
			{ 
			}

		public:
			ECSEntityID followedEntityID;
			glm::vec2 offsetXY;
			float offsetZ;
			float rotationSpeed;
			
			float prevX = 0.0f;
			float prevY = 0.0f;
			float cameraYaw = 0.0f;
			float cameraPitch = 0.0f;
		};

	}
}
