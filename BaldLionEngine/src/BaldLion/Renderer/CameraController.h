#pragma once
#include "BaldLion/Core/TimeStep.h"
#include "Camera.h"

namespace BaldLion
{
	class CameraController
	{

	public: 
		virtual void OnUpdate(BaldLion::TimeStep timeStep) = 0;				
	};


	class ProjectionCameraController : public CameraController
	{

	public:
		ProjectionCameraController(){}

		ProjectionCameraController(const glm::vec3& initialPosition, float width, float height, float farPlane, float nearPlane);

		ProjectionCameraController(const ProjectionCamera& camera);

		virtual void OnUpdate(BaldLion::TimeStep timeStep) override;

		ProjectionCamera& GetCamera() { return m_camera; }
		const ProjectionCamera& GetCamera() const { return m_camera; }

	private:
		void HandleCameraMovement(float deltaTime);

	private:
		ProjectionCamera m_camera;

		float m_prevX;
		float m_prevY;

		float m_cameraPitchRotation;
		float m_cameraYawRotation;

		float m_cameraRotationSpeed;
		float m_cameraMovementSpeed;
	};
}