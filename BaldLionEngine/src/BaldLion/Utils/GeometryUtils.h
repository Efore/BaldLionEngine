#pragma once
#include <glm/glm.hpp>
#include <assimp/aabb.h>
#include <glm/gtx/norm.hpp>

namespace BaldLion {

	namespace GeometryUtils {

		struct BoundingBox {

			glm::vec3 minPoint = glm::vec3(0.0f);
			glm::vec3 maxPoint = glm::vec3(0.0f);
			glm::vec3 center = glm::vec3(0.0f);

			inline const float GetRadius() const { return glm::length(maxPoint - minPoint) * 0.5f; }
			inline const float GetSqrRadius() const { return glm::length2((maxPoint - minPoint) * 0.5f); }

			static BoundingBox AiAABBToAABB(const aiAABB& aabb)
			{
				return { glm::vec3(aabb.mMin.x,aabb.mMin.y,aabb.mMin.z), glm::vec3(aabb.mMax.x,aabb.mMax.y,aabb.mMax.z) };
			}

			bool IsIntersectedByRayFast(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const
			{				
				const glm::vec3 toCentre = center - rayOrigin;			

				float proyectionDistance = glm::dot(glm::normalize(rayDirection), toCentre);

				float sqrRejection = glm::length2(toCentre) - (proyectionDistance * proyectionDistance);
				
				return sqrRejection < GetSqrRadius();
			}

			bool IsIntersectedByRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& hit) const
			{
				const float invX = 1.0f / rayDirection.x;
				const float invY = 1.0f / rayDirection.y;
				const float invZ = 1.0f / rayDirection.z;

				float tx1 = (minPoint.x - rayOrigin.x) * invX;
				float tx2 = (maxPoint.x - rayOrigin.x) * invX;
				float ty1 = (minPoint.y - rayOrigin.y) * invY;
				float ty2 = (maxPoint.y - rayOrigin.y) * invY;
				float tz1 = (minPoint.z - rayOrigin.z) * invZ;
				float tz2 = (maxPoint.z - rayOrigin.z) * invZ;

				float tmin = glm::max(glm::max(glm::min(tx1, tx2), glm::min(ty1, ty2)), glm::min(tz1, tz1));
				float tmax = glm::min(glm::min(glm::max(tx1, tx2), glm::max(ty1, ty2)), glm::max(tz1, tz1));
				
				// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
				if (tmax < 0)
				{
					hit = tmax;
					return false;
				}

				// if tmin > tmax, ray doesn't intersect AABB
				if (tmin > tmax)
				{
					hit = tmax;
					return false;
				}

				hit = tmin;

				return true;
			}
		};

		static BoundingBox GetAABB(const BoundingBox& localBoundingBox, const glm::mat4& transformMatrix)
		{
			glm::vec4 boundingBoxPoints[8];

			boundingBoxPoints[0] = glm::vec4(localBoundingBox.minPoint, 1.0f);
			boundingBoxPoints[1] = glm::vec4(localBoundingBox.minPoint.x, localBoundingBox.minPoint.y, localBoundingBox.maxPoint.z, 1.0f);
			boundingBoxPoints[2] = glm::vec4(localBoundingBox.maxPoint.x, localBoundingBox.minPoint.y, localBoundingBox.minPoint.z, 1.0f);
			boundingBoxPoints[3] = glm::vec4(localBoundingBox.maxPoint.x, localBoundingBox.minPoint.y, localBoundingBox.maxPoint.z, 1.0f);

			boundingBoxPoints[4] = glm::vec4(localBoundingBox.minPoint.x, localBoundingBox.maxPoint.y, localBoundingBox.maxPoint.z, 1.0f);
			boundingBoxPoints[5] = glm::vec4(localBoundingBox.maxPoint.x, localBoundingBox.maxPoint.y, localBoundingBox.minPoint.z, 1.0f);
			boundingBoxPoints[6] = glm::vec4(localBoundingBox.minPoint.x, localBoundingBox.maxPoint.y, localBoundingBox.minPoint.z, 1.0f);
			boundingBoxPoints[7] = glm::vec4(localBoundingBox.maxPoint, 1.0f);

			float minX = FLT_MAX;
			float minY = FLT_MAX;
			float minZ = FLT_MAX;

			float maxX = -FLT_MAX;
			float maxY = -FLT_MAX;
			float maxZ = -FLT_MAX;

			for (ui32 i = 0; i < 8; ++i)
			{
				boundingBoxPoints[i] = transformMatrix * boundingBoxPoints[i];

				minX = glm::min(boundingBoxPoints[i].x, minX);
				minY = glm::min(boundingBoxPoints[i].y, minY);
				minZ = glm::min(boundingBoxPoints[i].z, minZ);

				maxX = glm::max(boundingBoxPoints[i].x, maxX);
				maxY = glm::max(boundingBoxPoints[i].y, maxY);
				maxZ = glm::max(boundingBoxPoints[i].z, maxZ);
			}

			return { glm::vec3(minX,minY,minZ), glm::vec3(maxX,maxY,maxZ),  (glm::vec3(minX,minY,minZ) + glm::vec3(maxX,maxY,maxZ)) * 0.5f };
		}

		
	}
}