#include "blpch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace BaldLion
{
	namespace Rendering
	{
		//------------------------------------------------------------------------------------------
		//	CAMERA
		//-----------------------------------------------------------------------------------------------

		glm::vec3 Camera::GetForwardDirection()
		{
			return glm::vec3(m_cameraTransform[2][0], m_cameraTransform[2][1], m_cameraTransform[2][2]);
		}

		glm::vec3 Camera::GetRightDirection()
		{
			return glm::vec3(m_cameraTransform[0][0], m_cameraTransform[0][1], m_cameraTransform[0][2]);
		}

		//------------------------------------------------------------------------------------------
		//	PROJECTION CAMERA
		//-----------------------------------------------------------------------------------------------

		ProjectionCamera::ProjectionCamera(const glm::vec3& initialPosition, float width, float height, float nearPlane, float farPlane) :
			m_width(width), m_height(height), m_yaw(0.0f), m_pitch(0.0f)
		{
			m_position = initialPosition;
			m_farPlane = farPlane;
			m_nearPlane = nearPlane;
			RecalculateViewMatrix();
		}

		void ProjectionCamera::RecalculateViewMatrix()
		{
			BL_PROFILE_FUNCTION();

			m_cameraTransform = glm::translate(glm::mat4(1.0f), m_position) *
				glm::rotate(glm::mat4(1.0f), glm::radians(m_yaw), glm::vec3(0, 1, 0)) *
				glm::rotate(glm::mat4(1.0f), glm::radians(m_pitch), glm::vec3(1, 0, 0));

			m_viewMatrix = glm::inverse(m_cameraTransform);

			m_projectionMatrix = glm::perspective(glm::radians(45.0f), m_width / m_height, m_nearPlane, m_farPlane);

			m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
		}

		//------------------------------------------------------------------------------------------
		//	ORTHOGRAPHIC CAMERA
		//-----------------------------------------------------------------------------------------------

		OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		{
			m_projectionMatrix = (glm::ortho(left, right, bottom, top, -1.0f, 1.0f));
		}

		void OrthographicCamera::RecalculateViewMatrix()
		{
			BL_PROFILE_FUNCTION();

			glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position);
			m_viewMatrix = glm::inverse(transform);

			m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
		}
	}
}