#include "blpch.h"

#include "BaldLion/Core/Input.h"
#include "BaldLion/Core/KeyCodes.h"
#include "BaldLion/Core/MouseButtonCodes.h"

#include "CameraManager.h"

namespace BaldLion
{
	namespace Rendering
	{
		ProjectionCameraManager::ProjectionCameraManagerData ProjectionCameraManager::s_cameraData;
		bool ProjectionCameraManager::s_initialized = false;

		void ProjectionCameraManager::Init(const glm::vec3 & initialPosition, float width, float height, float nearPlane, float farPlane, float cameraMovementSpeed)
		{
			s_cameraData.cameraMovementSpeed = cameraMovementSpeed;
			s_cameraData.cameraRotationSpeed = 10.0f;
			s_cameraData.cameraYawRotation = 0.0f;
			s_cameraData.cameraPitchRotation = 0.0f;
			
			s_cameraData.prevX = BaldLion::Input::GetMouseX();
			s_cameraData.prevY = BaldLion::Input::GetMouseY();
			
			s_cameraData.camera = MemoryManager::New<ProjectionCamera>("Camera", AllocationType::FreeList_Renderer, initialPosition, width, height, nearPlane, farPlane);
		}

		void ProjectionCameraManager::SetCamera(ProjectionCamera* camera)
		{
			s_cameraData.camera = camera;
			
			s_cameraData.cameraYawRotation = 0.0f;
			s_cameraData.cameraPitchRotation = 0.0f;
			
			s_cameraData.prevX = BaldLion::Input::GetMouseX();
			s_cameraData.prevY = BaldLion::Input::GetMouseY();
		}

		void ProjectionCameraManager::Stop()
		{
			MemoryManager::Delete(s_cameraData.camera);
		}

		void ProjectionCameraManager::OnUpdate(BaldLion::TimeStep timeStep)
		{
			BL_PROFILE_FUNCTION();

			HandleCameraMovement(timeStep);
		}
		
		void ProjectionCameraManager::HandleCameraMovement(float deltaTime)
		{
			glm::vec3 cameraMovement = glm::vec3(0, 0, 0);

			if (BaldLion::Input::IsKeyPressed(BL_KEY_W))
				cameraMovement -= s_cameraData.camera->GetForwardDirection() * deltaTime * s_cameraData.cameraMovementSpeed;
			else if (BaldLion::Input::IsKeyPressed(BL_KEY_S))
				cameraMovement += s_cameraData.camera->GetForwardDirection() * deltaTime * s_cameraData.cameraMovementSpeed;

			if (BaldLion::Input::IsKeyPressed(BL_KEY_A))
				cameraMovement -= s_cameraData.camera->GetRightDirection() * deltaTime * s_cameraData.cameraMovementSpeed;
			else if (BaldLion::Input::IsKeyPressed(BL_KEY_D))
				cameraMovement += s_cameraData.camera->GetRightDirection() * deltaTime * s_cameraData.cameraMovementSpeed;

			if (BaldLion::Input::IsKeyPressed(BL_KEY_LEFT_SHIFT))
				cameraMovement *= 2;

			if (cameraMovement != glm::vec3(0, 0, 0))
				s_cameraData.camera->SetPosition(s_cameraData.camera->GetPosition() + cameraMovement);

			if (BaldLion::Input::IsMouseButtonPress(BL_MOUSE_BUTTON_2))
			{
				float deltaX = BaldLion::Input::GetMouseX() - s_cameraData.prevX;
				float deltaY = BaldLion::Input::GetMouseY() - s_cameraData.prevY;

				s_cameraData.cameraYawRotation -= deltaX * s_cameraData.cameraRotationSpeed * deltaTime;
				s_cameraData.cameraPitchRotation -= deltaY * s_cameraData.cameraRotationSpeed * deltaTime;
				s_cameraData.camera->SetRotation(s_cameraData.cameraPitchRotation, s_cameraData.cameraYawRotation);
			}

			s_cameraData.prevX = BaldLion::Input::GetMouseX();
			s_cameraData.prevY = BaldLion::Input::GetMouseY();
		}
	}
}