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

			const void * GetFirstElement() const{
				return &position.x;
			}
		};	

		struct VertexBoneData{

			glm::ivec3 jointIDs;
			glm::vec3 weights;

			const void * GetFirstElement() const{
				return &jointIDs.x;
			}
		};
	}
}