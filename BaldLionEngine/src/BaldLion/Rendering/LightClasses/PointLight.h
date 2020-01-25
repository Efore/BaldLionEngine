#pragma once
#include <glm/glm.hpp>

namespace BaldLion
{
	namespace Rendering
	{
		struct PointLight
		{
			glm::vec3 position;

			float constant;
			float linear;
			float quadratic;

			glm::vec3 ambientColor;
			glm::vec3 diffuseColor;
			glm::vec3 specularColor;
		};
	}
}