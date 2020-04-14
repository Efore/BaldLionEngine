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
			glm::vec2 tangent;
			glm::vec2 bitangent;

			float * GetFirstElement() {
				return &position.x;
			}
		};		

		const uint32_t NUM_BONES_PER_VEREX = 3;
		struct VertexBoneData
		{
			uint32_t vertexids[NUM_BONES_PER_VEREX];
			float weights[NUM_BONES_PER_VEREX];
		};
	}
}