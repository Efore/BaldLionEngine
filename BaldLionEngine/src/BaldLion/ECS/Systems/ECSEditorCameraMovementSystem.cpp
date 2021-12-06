#include "blpch.h"
#include "ECSEditorCameraMovementSystem.h"
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

		void ECSEditorCameraMovementSystem::OnStart()
		{
			for (ui32 i = 0; i < m_componentLookUps.Size(); ++i)
			{
				const ECSTransformComponent* cameraTransform = m_componentLookUps[i]->Read<ECSTransformComponent>(ECSComponentType::Transform);
				ECSProjectionCameraComponent* projectionCamera = m_componentLookUps[i]->Write<ECSProjectionCameraComponent>(ECSComponentType::ProjectionCamera);

				projectionCamera->cameraPitch = glm::degrees(cameraTransform->rotation.x);
				projectionCamera->cameraYaw = glm::degrees(cameraTransform->rotation.y);
			}
		}

		void ECSEditorCameraMovementSystem::UpdateOperation(TimeStep timeStep, ECSComponentLookUp* componentLookUp)
		{	
			BL_PROFILE_FUNCTION();

			ECSTransformComponent* cameraTransform = componentLookUp->Write<ECSTransformComponent>(ECSComponentType::Transform);
			ECSProjectionCameraComponent* projectionCamera = componentLookUp->Write<ECSProjectionCameraComponent>(ECSComponentType::ProjectionCamera);

			glm::mat4 cameraMatrixTransform = cameraTransform->GetTransformMatrix();

			glm::vec3 cameraMovement;
			CalculateCameraMovement(timeStep, projectionCamera->cameraMovementSpeed, cameraMatrixTransform, cameraMovement);
			CalculateCameraRotation(timeStep, projectionCamera->cameraRotationSpeed, projectionCamera->prevX, projectionCamera->prevY, projectionCamera->cameraYaw, projectionCamera->cameraPitch);

			cameraTransform->position += cameraMovement;
			cameraTransform->rotation = glm::vec3(glm::radians(projectionCamera->cameraPitch),glm::radians(projectionCamera->cameraYaw), 0.0f);

			cameraMatrixTransform = cameraTransform->GetTransformMatrix();

			const glm::mat4 viewMatrix = glm::inverse(cameraMatrixTransform);
			const glm::mat4 projectionMatrix = glm::perspective(glm::radians(projectionCamera->fov), projectionCamera->width / projectionCamera->height, projectionCamera->nearPlane, projectionCamera->farPlane);

			projectionCamera->viewProjectionMatrix = projectionMatrix * viewMatrix;			
		}

		void ECSEditorCameraMovementSystem::CalculateCameraMovement(const float deltaTime, const float cameraMovementSpeed,  const glm::mat4& cameraTransform, glm::vec3& cameraMovement)
		{
			cameraMovement = glm::vec3(0, 0, 0);			

			if (BaldLion::Input::IsMouseButtonPress(BL_MOUSE_BUTTON_2))
			{
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
		}

		void ECSEditorCameraMovementSystem::CalculateCameraRotation(const float deltaTime, const float cameraRotationSpeed, float& prevX, float& prevY, float& cameraYaw, float& cameraPitch)
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
