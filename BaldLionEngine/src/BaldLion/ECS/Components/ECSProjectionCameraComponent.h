#pragma once
#include "BaldLion/ECS/ECSComponent.h"
#include <glm/glm.hpp>

namespace BaldLion {

	namespace ECS
	{
		class ECSProjectionCameraComponent : public ECSComponent
		{
		public:

			ECSProjectionCameraComponent(float fov, float width, float height, float nearPlane, float farPlane) :
				ECSComponent(ECSComponentType::ProjectionCamera),
				fov(fov),
				width(width),
				height(height),
				nearPlane(nearPlane),
				farPlane(farPlane)
			{ 
			}
			
		public:
			glm::mat4 viewProjectionMatrix;
			glm::mat4 shadowProjectionMatrix;

			float fov;

			float width;
			float height;

			float nearPlane;
			float farPlane;
		};
	}
}