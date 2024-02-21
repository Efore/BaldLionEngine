#pragma once
#include "BaldLion/ECS/Components/ECSProjectionCameraComponent.h"
#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/Core/Containers/DynamicArray.h"
#include "BaldLion/Utils/GeometryUtils.h"

namespace BaldLion {

	namespace ECS {

		namespace SingletonSystems {

			class CameraSystem {

			public:

				static void Init();
				static void Stop();
				static void SetMainCamera(ECSProjectionCameraComponent* cameraComponent, ECSTransformComponent* cameraTransform);
				static void SetCameraSize(float width, float height);

				static const glm::mat4& GetMainCameraViewProjectionMatrix() { return s_mainCamera->viewProjectionMatrix; }
				static const glm::vec3& GetMainCameraPosition() { return s_mainCameraTransform->position; }
				
				static ECSTransformComponent* GetMainCameraTransform() { return s_mainCameraTransform; }

				static ECSProjectionCameraComponent* GetMainCamera() { return s_mainCamera; }

				static bool IsAABBVisible(const GeometryUtils::BoundingBox& aabb);
				static bool IsAABBShadowVisible(const GeometryUtils::BoundingBox& aabb);
				static void UpdateFrustrumPlanes();

			private:
				static ECSProjectionCameraComponent* s_mainCamera;
				static ECSTransformComponent* s_mainCameraTransform;
				static DynamicArray<glm::vec4> s_frustrumPlanes;
				static DynamicArray<glm::vec4> s_shadowFrustrumPlanes;
				static bool s_initialized ;
			};

		}

	}

}
