#pragma once
#include "BaldLion/Core/TimeStep.h"
#include "BaldLion/Events/ApplicationEvent.h"
#include "Camera.h"

namespace BaldLion
{
	namespace Rendering
	{
		class CameraController
		{

		public:
			virtual void OnUpdate(BaldLion::TimeStep timeStep) = 0;
		};


		class ProjectionCameraController : public CameraController
		{

		public:
			ProjectionCameraController() {}

			ProjectionCameraController(const glm::vec3& initialPosition, float width, float height, float nearPlane, float farPlane, float cameraMovementSpeed);

			ProjectionCameraController(const Ref<ProjectionCamera>& camera);

			virtual void OnUpdate(BaldLion::TimeStep timeStep) override;

			Ref<ProjectionCamera>& GetCamera() { return m_camera; }
			const Ref<ProjectionCamera>& GetCamera() const { return m_camera; }

		private:
			void HandleCameraMovement(float deltaTime);

		private:
			Ref<ProjectionCamera> m_camera;

			float m_prevX;
			float m_prevY;

			float m_cameraPitchRotation;
			float m_cameraYawRotation;

			float m_cameraRotationSpeed;
			float m_cameraMovementSpeed;
		};
	}
}