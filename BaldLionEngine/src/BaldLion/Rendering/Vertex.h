#pragma once
#include <glm/glm.hpp>

namespace BaldLion
{
	namespace Rendering
	{
		struct Vertex {

			glm::vec3 position;
			glm::vec3 color;
			glm::vec3 normal;
			glm::vec2 textCoord;
			glm::vec3 tangent;
			glm::vec3 bitangent;

			float * GetFirstElement() {
				return &position.x;
			}
		};	

		struct AnimatedVertex : public Vertex {

			glm::ivec3 jointIDs;
			glm::vec3 weights;
		};
	}
}