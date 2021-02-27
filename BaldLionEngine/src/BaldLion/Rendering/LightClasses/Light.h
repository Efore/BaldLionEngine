#pragma once
#include <glm/glm.hpp>

namespace BaldLion
{
	namespace Rendering
	{
		struct Light
		{
			glm::vec3 ambientColor;
			glm::vec3 diffuseColor;
			glm::vec3 specularColor;
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