#pragma once
#include "BaldLion/ECS/ECSSystem.h"
#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/ECS/Components/ECSProjectionCameraComponent.h"


namespace BaldLion {

	namespace ECS {

		class ECSCameraMovementSystem : public ECSSystem
		{
		public:
			ECSCameraMovementSystem(const char* systemName, const ECSSignature& signature, class ECSManager* ecsManager) :
				ECSSystem(systemName, signature, ecsManager, true, false) {}

			virtual void OnStart() override;
			virtual void UpdateOperation(TimeStep timeStep, ECSComponentLookUp* componentLookUp) override;
			virtual void OnStop() override {};

		private:
			void CalculateCameraMovement(const float deltaTime, const float cameraMovementSpeed, const glm::mat4& cameraTransform, glm::vec3& cameraMovement);
			void CalculateCameraRotation(const float deltaTime, const float cameraRotationSpeed, float& prevX, float& prevY, float& cameraYaw, float& cameraPitch);			
		};

	}
}
