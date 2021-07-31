#include "blpch.h"
#include "ECSCameraMovementSystem.h"

#include "BaldLion/Core/Input.h"
#include "BaldLion/Core/KeyCodes.h"
#include "BaldLion/Core/MouseButtonCodes.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace BaldLion {
	namespace ECS {

		void ECSCameraMovementSystem::OnUpdate(TimeStep timeStep)
		{
			const float deltaTime = timeStep.GetSeconds();

			for (ui32 i = 0; i < m_componentLookUps.Size(); ++i)
			{
				ECSTransformComponent* cameraTransform = m_componentLookUps[i]->Write<ECSTransformComponent>(ECSComponentID::Transform);
				ECSProjectionCameraComponent* projectionCamera = m_componentLookUps[i]->Write<ECSProjectionCameraComponent>(ECSComponentID::ProjectionCamera);

				glm::mat4 cameraMatrixTransform = cameraTransform->GetTransformMatrix();

				glm::vec3 cameraMovement;
				CalculateCameraMovement(timeStep.GetSeconds(), projectionCamera->cameraMovementSpeed, cameraMatrixTransform, cameraMovement);
				
				glm::vec3 eulerRotation = glm::eulerAngles(cameraTransform->rotation);
				float cameraYaw = eulerRotation.y;
				float cameraPitch = eulerRotation.x; 

				CalculateCameraRotation(timeStep.GetSeconds(), projectionCamera->cameraRotationSpeed, projectionCamera->prevX, projectionCamera->prevY, cameraYaw, cameraPitch);

				cameraMatrixTransform = glm::translate(cameraMatrixTransform, cameraMovement) *
					glm::rotate(glm::mat4(1.0f), glm::radians(cameraYaw), glm::vec3(0, 1, 0)) *
					glm::rotate(glm::mat4(1.0f), glm::radians(cameraPitch), glm::vec3(1, 0, 0));

				glm::vec3 skew;
				glm::vec4 perspective;
				glm::decompose(cameraMatrixTransform, cameraTransform->scale, cameraTransform->rotation, cameraTransform->position, skew, perspective);
			}
		}

		void ECSCameraMovementSystem::CalculateCameraMovement(const float deltaTime, const float cameraMovementSpeed,  const glm::mat4& cameraTransform, glm::vec3& cameraMovement)
		{
			cameraMovement = glm::vec3(0, 0, 0);			

			if (BaldLion::Input::IsKeyPressed(BL_KEY_W))
				cameraMovement -= GetForwardDirection(cameraTransform) * deltaTime * cameraMovementSpeed;
			else if (BaldLion::Input::IsKeyPressed(BL_KEY_S))
				cameraMovement += GetForwardDirection(cameraTransform) * deltaTime * cameraMovementSpeed;

			if (BaldLion::Input::IsKeyPressed(BL_KEY_A))
				cameraMovement -= GetRightDirection(cameraTransform) * deltaTime * cameraMovementSpeed;
			else if (BaldLion::Input::IsKeyPressed(BL_KEY_D))
				cameraMovement += GetRightDirection(cameraTransform) * deltaTime * cameraMovementSpeed;

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

		const glm::vec3 ECSCameraMovementSystem::GetForwardDirection(const glm::mat4& transformMatrix) const
		{	
			return glm::vec3(transformMatrix[2][0], transformMatrix[2][1], transformMatrix[2][2]);
		}

		const glm::vec3 ECSCameraMovementSystem::GetRightDirection(const glm::mat4& transformMatrix) const
		{
			return glm::vec3(transformMatrix[0][0], transformMatrix[0][1], transformMatrix[0][2]);
		}

	}
}
