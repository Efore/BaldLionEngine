#include "blpch.h"
#include "ECSCameraMovementSystem.h"
#include "BaldLion/Utils/MathUtils.h"
#include "BaldLion/Core/Input.h"
#include "BaldLion/Core/KeyCodes.h"
#include "BaldLion/Core/MouseButtonCodes.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace BaldLion {

	namespace ECS {

		void ECSCameraMovementSystem::OnStart()
		{
			for (ui32 i = 0; i < m_componentLookUps.Size(); ++i)
			{
				const ECSTransformComponent* cameraTransform = m_componentLookUps[i]->Read<ECSTransformComponent>(ECSComponentID::Transform);
				ECSProjectionCameraComponent* projectionCamera = m_componentLookUps[i]->Write<ECSProjectionCameraComponent>(ECSComponentID::ProjectionCamera);

				projectionCamera->cameraPitch = glm::degrees(glm::pitch(cameraTransform->rotation));
				projectionCamera->cameraYaw = glm::degrees(glm::yaw(cameraTransform->rotation));
			}
		}

		void ECSCameraMovementSystem::UpdateOperation(TimeStep timeStep, ECSComponentLookUp* componentLookUp)
		{	
			ECSTransformComponent* cameraTransform = componentLookUp->Write<ECSTransformComponent>(ECSComponentID::Transform);
			ECSProjectionCameraComponent* projectionCamera = componentLookUp->Write<ECSProjectionCameraComponent>(ECSComponentID::ProjectionCamera);

			glm::mat4 cameraMatrixTransform = cameraTransform->GetTransformMatrix();

			glm::vec3 cameraMovement;
			CalculateCameraMovement(timeStep, projectionCamera->cameraMovementSpeed, cameraMatrixTransform, cameraMovement);
			CalculateCameraRotation(timeStep, projectionCamera->cameraRotationSpeed, projectionCamera->prevX, projectionCamera->prevY, projectionCamera->cameraYaw, projectionCamera->cameraPitch);

			cameraTransform->position += cameraMovement;
			cameraTransform->rotation = glm::quat_cast(glm::yawPitchRoll(glm::radians(projectionCamera->cameraYaw), glm::radians(projectionCamera->cameraPitch), 0.0f));

			cameraMatrixTransform = cameraTransform->GetTransformMatrix();

			const glm::mat4 viewMatrix = glm::inverse(cameraMatrixTransform);
			const glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), projectionCamera->width / projectionCamera->height, projectionCamera->nearPlane, projectionCamera->farPlane);

			projectionCamera->viewProjectionMatrix = projectionMatrix * viewMatrix;			
		}

		void ECSCameraMovementSystem::CalculateCameraMovement(const float deltaTime, const float cameraMovementSpeed,  const glm::mat4& cameraTransform, glm::vec3& cameraMovement)
		{
			cameraMovement = glm::vec3(0, 0, 0);			

			if (BaldLion::Input::IsKeyPressed(BL_KEY_W))
				cameraMovement -= MathUtils::GetTransformForwardDirection(cameraTransform) * deltaTime * cameraMovementSpeed;
			else if (BaldLion::Input::IsKeyPressed(BL_KEY_S))
				cameraMovement += MathUtils::GetTransformForwardDirection(cameraTransform) * deltaTime * cameraMovementSpeed;

			if (BaldLion::Input::IsKeyPressed(BL_KEY_A))
				cameraMovement -= MathUtils::GetTransformRightDirection(cameraTransform) * deltaTime * cameraMovementSpeed;
			else if (BaldLion::Input::IsKeyPressed(BL_KEY_D))
				cameraMovement += MathUtils::GetTransformRightDirection(cameraTransform) * deltaTime * cameraMovementSpeed;

			if (BaldLion::Input::IsKeyPressed(BL_KEY_LEFT_SHIFT))
				cameraMovement *= 2;
		}

		void ECSCameraMovementSystem::CalculateCameraRotation(const float deltaTime, const float cameraRotationSpeed, float& prevX, float& prevY, float& cameraYaw, float& cameraPitch)
		{
			if (BaldLion::Input::IsMouseButtonPress(BL_MOUSE_BUTTON_2))
			{
				float deltaX = BaldLion::Input::GetMouseX() - prevX;
				float deltaY = BaldLion::Input::GetMouseY() - prevY;

				cameraYaw -= deltaX * cameraRotationSpeed * deltaTime;
				cameraPitch -= deltaY * cameraRotationSpeed * deltaTime;
			}

			prevX = BaldLion::Input::GetMouseX();
			prevY = BaldLion::Input::GetMouseY();
		}

	}
}
