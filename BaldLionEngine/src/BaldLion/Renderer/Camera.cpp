#include "blpch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace BaldLion
{
	//------------------------------------------------------------------------------------------
	//	PROJECTION CAMERA
	//-----------------------------------------------------------------------------------------------

	ProjectionCamera::ProjectionCamera( float width, float height, float farPlane, float nearPlane) :
		m_width(width), m_height(height)		
	{
		m_farPlane = farPlane;
		m_nearPlane = nearPlane;
	}

	void ProjectionCamera::RecalculateViewMatrix()
	{
		m_viewMatrix = glm::lookAt(
			m_position,
			m_direction,
			glm::vec3(0, 1, 0)
		);

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
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position);
		m_viewMatrix = glm::inverse(transform);

		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}



}