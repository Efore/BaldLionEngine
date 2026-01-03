#pragma once
#include <glm/glm.hpp>

namespace BaldLion
{
	namespace Rendering
	{
		struct Vertex {

			glm::vec3 position = glm::vec3(0.0f);			
			glm::vec3 normal = glm::vec3(0.0f);
			glm::vec3 tangent = glm::vec3(0.0f);
			glm::vec2 textCoord = glm::vec2(0.0f);

			const void * GetFirstElement() const{
				return &position.x;
			}

			Vertex(){

			}

			Vertex(const glm::vec3& pos, const glm::vec3& nor, const glm::vec2& tCoords, const glm::vec3& tan) :
				position(pos),
				normal(nor),
				textCoord(tCoords),
				tangent(tan)
			{
			}

			Vertex(const Vertex& other) : 
				position(other.position),
				normal(other.normal),
				textCoord(other.textCoord),
				tangent(other.tangent)
			{
			}

			Vertex& operator= (const Vertex& other)
			{
				position = other.position;
				normal = other.normal;
				textCoord = other.textCoord;
				tangent = other.tangent;
				
				return *this;
			}			

			Vertex& operator* (const glm::mat4& transform)
			{
				position = transform * glm::vec4(position,1.0f);
				normal = transform * glm::vec4(normal, 1.0f);
				tangent = transform * glm::vec4(tangent, 1.0f);
				return *this;
			}

			friend Vertex operator* (const Vertex& vertex, const glm::mat4& transform)
			{
				Vertex result = vertex;
				result.position = transform * glm::vec4(vertex.position,1.0f);
				result.normal = transform * glm::vec4(vertex.normal, 1.0f);
				result.tangent = transform * glm::vec4(vertex.tangent, 1.0f);

				return result;
			}
		};	

		struct VertexBone{

			VertexBone() {
				jointIDs = glm::ivec3(0);
				weights = glm::vec3(0.0f);
			}

			glm::ivec3 jointIDs;
			glm::vec3 weights;

			const void * GetFirstElement() const{
				return &jointIDs.x;
			}
		};
	}
}