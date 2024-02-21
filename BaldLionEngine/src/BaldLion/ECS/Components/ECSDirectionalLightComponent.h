#pragma once
#include "BaldLion/ECS/ECSComponent.h"
#include <glm/glm.hpp>

namespace BaldLion {

	namespace ECS
	{
		class ECSDirectionalLightComponent : public ECSComponent
		{
		public:

			ECSDirectionalLightComponent(const glm::vec3& ambientColor, const glm::vec3& diffuseColor, const glm::vec3& specularColor) :
				ECSComponent(ECSComponentType::DirectionalLight),
				ambientColor(ambientColor),
				diffuseColor(diffuseColor),
				specularColor(specularColor)
			{
			}

		public:
			glm::vec3 ambientColor;
			glm::vec3 diffuseColor;
			glm::vec3 specularColor;
		};
	}
}
