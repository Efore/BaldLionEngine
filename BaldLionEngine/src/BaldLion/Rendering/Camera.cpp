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

		const bool Camera::IsAABBVisible(const GeometryUtils::AABB& aabb, const glm::mat4& worldTransform) const
		{
			const float scale = worldTransform[0].x;
			const glm::vec3 position = worldTransform[3];

			const glm::vec3 minPointInWorldSpace = (aabb.minPoint * scale) + position;
			const glm::vec3 maxPointInWorldSpace = (aabb.maxPoint * scale) + position;

			glm::vec3 axisVert;

			for (ui32 i = 0; i < m_frustrumPlanes.Size(); ++i)
			{
				glm::vec3 planeNormal(m_frustrumPlanes[i].x, m_frustrumPlanes[i].y, m_frustrumPlanes[i].z);
				float planeConstant = m_frustrumPlanes[i].w;

				axisVert.x = m_frustrumPlanes[i].x < 0.0f ? minPointInWorldSpace.x : maxPointInWorldSpace.x;
				axisVert.y = m_frustrumPlanes[i].y < 0.0f ? minPointInWorldSpace.y : maxPointInWorldSpace.y;
				axisVert.z = m_frustrumPlanes[i].z < 0.0f ? minPointInWorldSpace.z : maxPointInWorldSpace.z;

				if (glm::dot(planeNormal, axisVert) + planeConstant < 0.0f)
					return false;
			}

			return true;
		}

		glm::vec3 Camera::GetForwardDirection()
		{
			return glm::vec3(m_cameraTransform[2][0], m_cameraTransform[2][1], m_cameraTransform[2][2]);
		}

		glm::vec3 Camera::GetRightDirection()
		{
			return glm::vec3(m_cameraTransform[0][0], m_cameraTransform[0][1], m_cameraTransform[0][2]);
		}

		void Camera::UpdateFrustrumPlanes()
		{
			glm::mat4 transposedViewProjection = glm::transpose(m_viewProjectionMatrix);

			m_frustrumPlanes[0] = transposedViewProjection[3] + transposedViewProjection[0];	//Left Plane
			m_frustrumPlanes[1] = transposedViewProjection[3] - transposedViewProjection[0];	//Right Plane
			m_frustrumPlanes[2] = transposedViewProjection[3] - transposedViewProjection[1];	//Top Plane
			m_frustrumPlanes[3] = transposedViewProjection[3] + transposedViewProjection[1];	//Bottom Plane
			m_frustrumPlanes[4] = transposedViewProjection[3] + transposedViewProjection[2];	//Near Plane
			m_frustrumPlanes[5] = transposedViewProjection[3] - transposedViewProjection[2];	//Far Plane	

			////Normalizing planes
			for (ui32 i = 0; i < m_frustrumPlanes.Size(); ++i)
			{
				float magnitude = glm::sqrt((m_frustrumPlanes[i].x * m_frustrumPlanes[i].x) + (m_frustrumPlanes[i].y * m_frustrumPlanes[i].y) + (m_frustrumPlanes[i].z * m_frustrumPlanes[i].z));
				m_frustrumPlanes[i].x /= magnitude;
				m_frustrumPlanes[i].y /= magnitude;
				m_frustrumPlanes[i].z /= magnitude;
				m_frustrumPlanes[i].w /= magnitude;
			}
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
			
			m_frustrumPlanes = DynamicArray<glm::vec4>(AllocationType::FreeList_Renderer, 6);
			m_frustrumPlanes.Populate();

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

			UpdateFrustrumPlanes();
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

			UpdateFrustrumPlanes();
		}
	}
}