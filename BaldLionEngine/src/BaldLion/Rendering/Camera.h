#pragma once

#include "BaldLion/Core/Containers/DynamicArray.h"
#include "BaldLion/Utils/GeometryUtils.h"
#include <glm/glm.hpp>

namespace BaldLion
{
	namespace Rendering
	{
		class Camera
		{
		public:
			void SetPosition(const glm::vec3& position) { m_position = position; }
			const inline glm::vec3& GetPosition() const { return m_position; }

			const inline glm::mat4& GetViewProjectionMatrix() const { return m_viewProjectionMatrix; }
			const inline glm::mat4& GetViewMatrix() const { return m_viewMatrix; }
			const inline glm::mat4& GetProjectionMatrix() const { return m_projectionMatrix; }

			void SetFarPlane(float farPlane) { m_farPlane = farPlane; }
			inline float GetFarPlane() const { return m_farPlane; }

			void SetNearPlane(float nearPlane) { m_nearPlane = nearPlane; }
			inline float GetNearPlane() const { return m_nearPlane; }

			const bool IsAABBVisible(const GeometryUtils::AABB& aabb) const;

			glm::vec3 GetForwardDirection();
			glm::vec3 GetRightDirection();

			virtual void RecalculateViewMatrix() = 0;

		protected:
			void UpdateFrustrumPlanes();

		protected:
			glm::mat4 m_projectionMatrix;
			glm::mat4 m_viewMatrix;
			glm::mat4 m_cameraTransform;
			glm::mat4 m_viewProjectionMatrix;
			glm::vec3 m_position;
			DynamicArray<glm::vec4> m_frustrumPlanes;
			

			float m_farPlane;
			float m_nearPlane;
		};

		class ProjectionCamera : public Camera
		{
		public:
			ProjectionCamera() {}
			ProjectionCamera(const glm::vec3& initialPosition, float width, float height, float nearPlane, float farPlane);

			const inline glm::vec3& GetDirection() const { return m_direction; }

			void SetWidth(float width) { m_width = width; }
			inline float GetWidth() const { return m_width; }

			void SetHeight(float height) { m_height = height; }
			inline float GetHeight() const { return m_height; }

			void SetRotation(float pitch, float yaw) { m_pitch = pitch; m_yaw = yaw; }
			inline float GetPitch() const { return m_pitch; }
			inline float GetYaw() const { return m_yaw; }

			virtual void RecalculateViewMatrix() override;		

		private:
			glm::vec3 m_direction;

			float m_width;
			float m_height;

			float m_pitch;
			float m_yaw;
		};

		class OrthographicCamera : public Camera
		{
		public:

			OrthographicCamera(float left, float right, float bottom, float top);
			virtual void RecalculateViewMatrix() override;
		
		};
	}
}