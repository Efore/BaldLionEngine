#pragma once
#include "BaldLion/Core/TimeStep.h"
#include "BaldLion/Events/ApplicationEvent.h"
#include "Camera.h"

namespace BaldLion
{
	namespace Rendering
	{
		class ProjectionCameraManager 
		{

		public:
			static void Init(const glm::vec3& initialPosition, float width, float height, float nearPlane, float farPlane, float cameraMovementSpeed);
			static void SetCamera(const Ref<ProjectionCamera>& camera);

			static void OnUpdate(BaldLion::TimeStep timeStep);

			static const Ref<ProjectionCamera>& GetCamera() { return s_cameraData.camera; }

		private:

			static void HandleCameraMovement(float deltaTime);

		private:

			struct ProjectionCameraManagerData
			{
				Ref<ProjectionCamera> camera;

				float prevX;
				float prevY;

				float cameraPitchRotation;
				float cameraYawRotation;

				float cameraRotationSpeed;
				float cameraMovementSpeed;
			};

			static ProjectionCameraManagerData s_cameraData;
			static bool s_initialized;
			
		};
	}
}