#pragma once
#include "BaldLion/ECS/Components/ECSProjectionCameraComponent.h"
#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/Core/Containers/DynamicArray.h"
#include "BaldLion/Utils/GeometryUtils.h"

namespace BaldLion {

	namespace ECS {

		namespace SingletonComponents {

			class ECSProjectionCameraSingleton {

			public:

				static void Init();
				static void SetMainCamera(ECSProjectionCameraComponent* cameraComponent, ECSTransformComponent* cameraTransform);
				static bool IsAABBVisible(const GeometryUtils::AABB& aabb);
				static void UpdateFrustrumPlanes();

				static const glm::mat4& GetMainCameraViewProjectionMatrix() { return s_mainCamera->viewProjectionMatrix; }
				static const glm::vec3& GetMainCameraPosition() { return s_mainCameraTransform->position; }

			private:
				static ECSProjectionCameraComponent* s_mainCamera;
				static ECSTransformComponent* s_mainCameraTransform;
				static DynamicArray<glm::vec4> s_frustrumPlanes;
			};

		}

	}

}
