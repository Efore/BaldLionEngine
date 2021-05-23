#pragma once
#include <glm/glm.hpp>

namespace BaldLion
{
	namespace Rendering
	{
		enum class LightType
		{
			DirectionalLight,
			PointLight,
			SpotLight,

			Count
		};

		struct Light
		{
			glm::vec3 ambientColor;
			glm::vec3 diffuseColor;
			glm::vec3 specularColor;
			LightType lightType;
		};

		struct DirectionalLight : public Light
		{
			glm::vec3 direction;
		};

		struct PointLight : public Light
		{
			glm::vec3 position;
			float constant;
			float linear;
			float quadratic;
		};
	}
}