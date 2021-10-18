#pragma once
#include "BaldLion/ECS/ECSComponent.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


namespace BaldLion {

	namespace ECS
	{
		class ECSTransformComponent : public ECSComponent
		{
		public:

			ECSTransformComponent(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) : 
				ECSComponent(ECSComponentID::Transform),
				position(position), 
				rotation(rotation), 
				scale(scale)
			{ 
			}

			const glm::mat4 GetTransformMatrix() const {

				return glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f), scale);
			}


		public:
			glm::quat rotation;
			glm::vec3 position;
			glm::vec3 scale;

		};

	}
}
