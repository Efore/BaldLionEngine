#pragma once
#include <glm/glm.hpp>

namespace BaldLion
{
	struct Vertex {

		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 textCoord;

		float * GetFirstElement() {
			return &position.x;
		}
	};	
}