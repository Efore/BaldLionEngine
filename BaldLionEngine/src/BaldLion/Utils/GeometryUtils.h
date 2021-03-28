#pragma once
#include <glm/glm.hpp>
#include <assimp/aabb.h>

namespace BaldLion {

	namespace GeometryUtils {

		struct AABB {

			glm::vec3 minPoint;
			glm::vec3 maxPoint;

			inline const glm::vec3 GetCenter() const { return ((maxPoint + minPoint) / 2.0f); }
			inline const float GetRadius() const { return glm::length((maxPoint + minPoint) / 2.0f); }

			static AABB AiAABBToAABB(const aiAABB& aabb)
			{
				return { glm::vec3(aabb.mMin.x,aabb.mMin.y,aabb.mMin.z), glm::vec3(aabb.mMax.x,aabb.mMax.y,aabb.mMax.z) };
			}
		};

	}
}