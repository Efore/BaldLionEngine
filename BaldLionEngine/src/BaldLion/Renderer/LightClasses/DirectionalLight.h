#pragma once
#include <glm/glm.hpp>

namespace BaldLion
{
	struct DirectionalLight
	{
		glm::vec3 direction;

		glm::vec3 ambientColor;
		glm::vec3 diffuseColor;
		glm::vec3 specularColor;
	};
}