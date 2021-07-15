#pragma once
#include "BaldLion/ECS/ECSSystem.h"
#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/ECS/Components/ECSProjectionCameraComponent.h"


namespace BaldLion {
	namespace ECS {

		class ECSCameraMovementSystem : public ECSSystem
		{
		public:
			virtual void OnStart() override {};
			virtual void OnUpdate(TimeStep timeStep) override;
			virtual void OnDestroy() override {};

		private:
			void CalculateCameraMovement(const float deltaTime, const float cameraMovementSpeed, const glm::mat4& cameraTransform, glm::vec3& cameraMovement);
			void CalculateCameraRotation(const float deltaTime, const float cameraRotationSpeed, float& prevX, float& prevY, float& cameraYaw, float& cameraPitch);

			const glm::vec3 GetForwardDirection(const glm::mat4& transformMatrix) const;
			const glm::vec3 GetRightDirection(const glm::mat4& transformMatrix) const;
			
		};

	}
}
