#pragma once
#include "BaldLion/Core/TimeStep.h"
#include "BaldLion/Events/ApplicationEvent.h"
#include "Camera.h"

namespace BaldLion
{
	namespace Rendering
	{
		class CameraManager
		{

		public:
			virtual void OnUpdate(BaldLion::TimeStep timeStep) = 0;
		};


		class ProjectionCameraManager : public CameraManager
		{

		public:
			~ProjectionCameraManager() {}
		
			static ProjectionCameraManager* GetInstance();

			void SetUpInitialValues(const glm::vec3& initialPosition, float width, float height, float nearPlane, float farPlane, float cameraMovementSpeed);
			void SetCamera(const Ref<ProjectionCamera>& camera);

			virtual void OnUpdate(BaldLion::TimeStep timeStep) override;

			Ref<ProjectionCamera>& GetCamera() { return m_camera; }
			const Ref<ProjectionCamera>& GetCamera() const { return m_camera; }

		private:

			ProjectionCameraManager() {}

			void HandleCameraMovement(float deltaTime);

		private:

			static ProjectionCameraManager* s_instance;

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