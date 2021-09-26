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
			glm::vec3 tangent;
			glm::vec2 textCoord;

			const void * GetFirstElement() const{
				return &position.x;
			}

			Vertex(){

			}

			Vertex(const glm::vec3& pos, const glm::vec3& col, const glm::vec3& nor, const glm::vec2& tCoords, const glm::vec3& tan) :
				position(pos),
				color(col),
				normal(nor),
				textCoord(tCoords),
				tangent(tan)
			{
			}

			Vertex(const Vertex& other) : 
				position(other.position),
				color(other.color),
				normal(other.normal),
				textCoord(other.textCoord),
				tangent(other.tangent)
			{
			}

			Vertex& operator= (const Vertex& other)
			{
				position = other.position;
				color = other.color;
				normal = other.normal;
				textCoord = other.textCoord;
				tangent = other.tangent;
				
				return *this;
			}			

			Vertex& operator* (const glm::mat4& transform)
			{
				position = glm::vec3(transform * glm::vec4(position, 1.0f));
				return *this;
			}

			friend Vertex operator* (const Vertex& vertex, const glm::mat4& transform)
			{
				BL_DEEP_PROFILE_SCOPE("Vertex operator*", Optick::Category::Rendering);
				Vertex result = vertex;
				result.position = glm::vec3(transform * glm::vec4(vertex.position, 1.0f));
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