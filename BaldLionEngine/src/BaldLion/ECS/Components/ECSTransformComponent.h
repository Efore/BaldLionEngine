#pragma once
#include "BaldLion/ECS/ECSComponent.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


namespace BaldLion {

	namespace ECS
	{
		class ECSTransformComponent : public ECSComponent
		{
		public:

			ECSTransformComponent(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) :
				ECSComponent(ECSComponentType::Transform),
				position(position), 
				rotation(rotation), 
				scale(scale)
			{ 
			}

			const glm::mat4 GetTransformMatrix() const {

				return glm::translate(glm::mat4(1.0f), position) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
			}


		public:
			glm::vec3 rotation;
			glm::vec3 position;
			glm::vec3 scale;
		};

	}
}
