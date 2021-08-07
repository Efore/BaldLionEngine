#pragma once
#include "blpch.h"
#include "ECSProjectionCameraSingleton.h"

namespace BaldLion {
	namespace ECS {
		namespace SingletonComponents {

			ECSProjectionCameraComponent* ECSProjectionCameraSingleton::s_mainCamera;
			ECSTransformComponent* ECSProjectionCameraSingleton::s_mainCameraTransform;

			DynamicArray<glm::vec4> ECSProjectionCameraSingleton::s_frustrumPlanes;
			
			void ECSProjectionCameraSingleton::Init()
			{
				s_frustrumPlanes = DynamicArray<glm::vec4>(AllocationType::FreeList_Renderer, 6);
				s_frustrumPlanes.Populate();
			}

			void ECSProjectionCameraSingleton::UpdateFrustrumPlanes()
			{
				glm::mat4 transposedViewProjection = glm::transpose(s_mainCamera->viewProjectionMatrix);

				s_frustrumPlanes[0] = transposedViewProjection[3] + transposedViewProjection[0];	//Left Plane
				s_frustrumPlanes[1] = transposedViewProjection[3] - transposedViewProjection[0];	//Right Plane
				s_frustrumPlanes[2] = transposedViewProjection[3] - transposedViewProjection[1];	//Top Plane
				s_frustrumPlanes[3] = transposedViewProjection[3] + transposedViewProjection[1];	//Bottom Plane
				s_frustrumPlanes[4] = transposedViewProjection[3] + transposedViewProjection[2];	//Near Plane
				s_frustrumPlanes[5] = transposedViewProjection[3] - transposedViewProjection[2];	//Far Plane	

				////Normalizing planes
				for (ui32 i = 0; i < s_frustrumPlanes.Size(); ++i)
				{
					float magnitude = glm::sqrt((s_frustrumPlanes[i].x * s_frustrumPlanes[i].x) + (s_frustrumPlanes[i].y * s_frustrumPlanes[i].y) + (s_frustrumPlanes[i].z * s_frustrumPlanes[i].z));
					s_frustrumPlanes[i].x /= magnitude;
					s_frustrumPlanes[i].y /= magnitude;
					s_frustrumPlanes[i].z /= magnitude;
					s_frustrumPlanes[i].w /= magnitude;
				}
			}

			void ECSProjectionCameraSingleton::SetMainCamera(ECSProjectionCameraComponent* cameraComponent, ECSTransformComponent* cameraTransform)
			{
				s_mainCamera = cameraComponent;
				s_mainCameraTransform = cameraTransform;
			}

			bool ECSProjectionCameraSingleton::IsAABBVisible(const GeometryUtils::AABB& aabb)
			{
				const glm::vec3 minPointInWorldSpace = aabb.minPoint;
				const glm::vec3 maxPointInWorldSpace = aabb.maxPoint;

				glm::vec3 axisVert;

				for (ui32 i = 0; i < s_frustrumPlanes.Size(); ++i)
				{
					glm::vec3 planeNormal(s_frustrumPlanes[i].x, s_frustrumPlanes[i].y, s_frustrumPlanes[i].z);
					float planeConstant = s_frustrumPlanes[i].w;

					axisVert.x = s_frustrumPlanes[i].x < 0.0f ? minPointInWorldSpace.x : maxPointInWorldSpace.x;
					axisVert.y = s_frustrumPlanes[i].y < 0.0f ? minPointInWorldSpace.y : maxPointInWorldSpace.y;
					axisVert.z = s_frustrumPlanes[i].z < 0.0f ? minPointInWorldSpace.z : maxPointInWorldSpace.z;

					if (glm::dot(planeNormal, axisVert) + planeConstant < 0.0f)
						return false;
				}

				return true;
			}
		}
	}
}
