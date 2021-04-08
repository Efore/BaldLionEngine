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

			Vertex& operator= (const Vertex& other)
			{
				position = other.position;
				color = other.color;
				normal = other.normal;
				textCoord = other.textCoord;
				tangent = other.tangent;
				bitangent = other.bitangent;

				return *this;
			}

			friend Vertex operator* (const Vertex& vertex, const glm::mat4 transform)
			{
				Vertex result = vertex;
				result.position = glm::vec4(vertex.position,1.0f) * transform;
				return result;
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