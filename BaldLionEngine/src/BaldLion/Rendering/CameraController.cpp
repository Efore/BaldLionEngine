#include "blpch.h"

#include "BaldLion/Core/Input.h"
#include "BaldLion/Core/KeyCodes.h"
#include "BaldLion/Core/MouseButtonCodes.h"

#include "CameraController.h"

namespace BaldLion
{
	namespace Rendering
	{
		ProjectionCameraController::ProjectionCameraController(const glm::vec3 & initialPosition, float width, float height, float nearPlane, float farPlane, float cameraMovementSpeed) : m_cameraMovementSpeed(cameraMovementSpeed)
		{
			m_cameraRotationSpeed = 10.0f;

			m_cameraYawRotation = 0.0f;
			m_cameraPitchRotation = 0.0f;

			m_prevX = BaldLion::Input::GetMouseX();
			m_prevY = BaldLion::Input::GetMouseY();

			m_camera = CreateRef<ProjectionCamera>(initialPosition, width, height, nearPlane, farPlane);
		}

		ProjectionCameraController::ProjectionCameraController(const Ref<ProjectionCamera>& camera)
		{
			m_camera = camera;

			m_cameraYawRotation = 0.0f;
			m_cameraPitchRotation = 0.0f;

			m_prevX = BaldLion::Input::GetMouseX();
			m_prevY = BaldLion::Input::GetMouseY();
		}

		void ProjectionCameraController::OnUpdate(BaldLion::TimeStep timeStep)
		{
			BL_PROFILE_FUNCTION();

			HandleCameraMovement(timeStep);
		}

		void ProjectionCameraController::HandleCameraMovement(float deltaTime)
		{
			glm::vec3 cameraMovement = glm::vec3(0, 0, 0);

			if (BaldLion::Input::IsKeyPressed(BL_KEY_W))
				cameraMovement -= m_camera->GetForwardDirection() * deltaTime * m_cameraMovementSpeed;
			else if (BaldLion::Input::IsKeyPressed(BL_KEY_S))
				cameraMovement += m_camera->GetForwardDirection() * deltaTime * m_cameraMovementSpeed;

			if (BaldLion::Input::IsKeyPressed(BL_KEY_A))
				cameraMovement -= m_camera->GetRightDirection() * deltaTime * m_cameraMovementSpeed;
			else if (BaldLion::Input::IsKeyPressed(BL_KEY_D))
				cameraMovement += m_camera->GetRightDirection() * deltaTime * m_cameraMovementSpeed;

			if (BaldLion::Input::IsKeyPressed(BL_KEY_LEFT_SHIFT))
				cameraMovement *= 2;

			if (cameraMovement != glm::vec3(0, 0, 0))
				m_camera->SetPosition(m_camera->GetPosition() + cameraMovement);

			if (BaldLion::Input::IsMouseButtonPress(BL_MOUSE_BUTTON_2))
			{
				float deltaX = BaldLion::Input::GetMouseX() - m_prevX;
				float deltaY = BaldLion::Input::GetMouseY() - m_prevY;

				m_cameraYawRotation -= deltaX * m_cameraRotationSpeed * deltaTime;
				m_cameraPitchRotation -= deltaY * m_cameraRotationSpeed * deltaTime;
				m_camera->SetRotation(m_cameraPitchRotation, m_cameraYawRotation);
			}

			m_prevX = BaldLion::Input::GetMouseX();
			m_prevY = BaldLion::Input::GetMouseY();
		}
	}
}