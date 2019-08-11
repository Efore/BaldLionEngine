#pragma once

#include <glm/glm.hpp>

namespace BaldLion
{
	class Camera
	{
	public:
		void SetPosition(const glm::vec3& position) {	m_position = position; RecalculateViewMatrix();	}
		const inline glm::vec3& GetPosition() const { return m_position; }

		const inline glm::mat4& GetViewProjectionMatrix() const { return m_viewProjectionMatrix; }
		const inline glm::mat4& GetViewMatrix() const { return m_viewMatrix; }
		const inline glm::mat4& GetProjectionMatrix() const { return m_projectionMatrix; }

		void SetFarPlane(float farPlane) { m_farPlane = farPlane; RecalculateViewMatrix(); }
		inline float GetFarPlane() const { return m_farPlane; }

		void SetNearPlane(float nearPlane) { m_nearPlane = nearPlane; RecalculateViewMatrix(); }
		inline float GetNearPlane() const { return m_nearPlane; }

		glm::vec3 GetForwardDirection();
		glm::vec3 GetRightDirection();

	protected:
		virtual void RecalculateViewMatrix() = 0;

	protected:
		glm::mat4 m_projectionMatrix;
		glm::mat4 m_viewMatrix;
		glm::mat4 m_cameraTransform;
		glm::mat4 m_viewProjectionMatrix;
		glm::vec3 m_position;		

		float m_farPlane;
		float m_nearPlane;
	};

	class ProjectionCamera : public Camera 
	{
	public:
		ProjectionCamera() {}
		ProjectionCamera(const glm::vec3& initialPosition, float width, float height, float farPlane, float nearPlane);

		void SetDirection(const glm::vec3& direction) { m_direction = direction; RecalculateViewMatrix(); }
		const inline glm::vec3& GetDirection() const { return m_direction; }

		void SetWidth(float width) { m_width = width; RecalculateViewMatrix(); }
		inline float GetWidth() const { return m_width; }

		void SetHeight(float height) { m_height = height; RecalculateViewMatrix(); }
		inline float GetHeight() const { return m_height; }

		void SetRotation(float pitch, float yaw) { m_pitch = pitch; m_yaw = yaw; RecalculateViewMatrix(); }
		inline float GetPitch() const { return m_pitch; }
		inline float GetYaw() const { return m_yaw; }

	protected:
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


	protected:
		virtual void RecalculateViewMatrix() override;
	};
}