#include "blpch.h"
#include "ECSCameraFollowSystem.h"

#include "BaldLion/SceneManagement/SceneManager.h"
#include "BaldLion/ECS/ECSManager.h"
#include "BaldLion/ECS/Components/ECSCameraFollowComponent.h"
#include "BaldLion/ECS/Components/ECSTransformComponent.h"

#include "BaldLion/Core/Input.h"
#include "BaldLion/Core/MouseButtonCodes.h"

#include "BaldLion/Utils/MathUtils.h"
#include <glm/gtx/vector_angle.hpp>


namespace BaldLion
{	
	namespace ECS
	{

		ECSCameraFollowSystem::ECSCameraFollowSystem(const char* systemName, const ECSSignature& signature, class ECSManager* ecsManager) : 
			ECSSystem(systemName, signature, ecsManager, true, true)
		{

		}

		void ECSCameraFollowSystem::UpdateComponents(ECSEntityID entityID, ECSComponentLookUp* componentLookUp, float deltaTime)
		{
			ECSCameraFollowComponent* cameraFollowComponent = componentLookUp->Write<ECSCameraFollowComponent>(ECSComponentType::CameraFollow);
			ECSTransformComponent* cameraTransformComponent = componentLookUp->Write<ECSTransformComponent>(ECSComponentType::Transform);

			if (cameraFollowComponent->followedEntityID > 0)
			{				
				CalculateInputRotation(cameraFollowComponent, deltaTime);

				const ECSTransformComponent* followedEntityTransform = m_ecsManager->GetEntityComponents().Get(cameraFollowComponent->followedEntityID).Read<ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform);
				const glm::mat4 cameraTransformMatrix = cameraTransformComponent->GetTransformMatrix();
				const glm::mat4 followedEntityTransformMatrix = followedEntityTransform->GetTransformMatrix();

				const glm::vec4 cameraCenterPosition = followedEntityTransformMatrix * glm::vec4(cameraFollowComponent->offsetXY, 0.0f,1.0f);
				
				const glm::vec4 initialCameraPosition = cameraCenterPosition - glm::vec4((MathUtils::GetTransformForwardDirection(followedEntityTransformMatrix) * cameraFollowComponent->offsetZ),1.0f);
				glm::vec4 stepArcBallPosition = initialCameraPosition;

				//Rotate the camera around the pivot point on the first axis.
				glm::mat4x4 rotationMatrixX(1.0f);
				rotationMatrixX = glm::rotate(rotationMatrixX, cameraFollowComponent->cameraYaw, MathUtils::Vector3UnitY);
				stepArcBallPosition = (rotationMatrixX * (initialCameraPosition - cameraCenterPosition)) + cameraCenterPosition;

				//Rotate the camera around the pivot point on the second axis.
				glm::mat4x4 rotationMatrixY(1.0f);
				rotationMatrixY = glm::rotate(rotationMatrixY, cameraFollowComponent->cameraPitch, MathUtils::GetTransformRightDirection(cameraTransformMatrix));
				stepArcBallPosition = (rotationMatrixY * (stepArcBallPosition - cameraCenterPosition)) + cameraCenterPosition;

				cameraTransformComponent->position = stepArcBallPosition;

				glm::vec3 directionToFollowedPosition = glm::normalize(glm::vec3(cameraCenterPosition) - cameraTransformComponent->position);
				const glm::quat lookAtQuat = glm::quatLookAt(directionToFollowedPosition, MathUtils::Vector3UnitY);

				cameraTransformComponent->rotation = glm::eulerAngles(lookAtQuat);
			}
		}	

		void ECSCameraFollowSystem::CalculateInputRotation(ECSCameraFollowComponent* cameraFollowComponent, float deltaTime)
		{
			const float difX = BaldLion::Input::GetMouseX() - cameraFollowComponent->prevX;
			const float difY = BaldLion::Input::GetMouseY() - cameraFollowComponent->prevY;

			cameraFollowComponent->prevX = BaldLion::Input::GetMouseX();
			cameraFollowComponent->prevY = BaldLion::Input::GetMouseY();

			float angleX = difX * deltaTime * cameraFollowComponent->rotationSpeed;
			float angleY = difY * deltaTime * cameraFollowComponent->rotationSpeed;

			cameraFollowComponent->cameraYaw += angleX;

			if (cameraFollowComponent->cameraPitch + angleY > 1.0f)
			{
				cameraFollowComponent->cameraPitch = 1.0f;
			}
			else if (cameraFollowComponent->cameraPitch + angleY < -1.0f)
			{
				cameraFollowComponent->cameraPitch = -1.0f;
			}
			else
			{
				cameraFollowComponent->cameraPitch += angleY;
			}
		}	

	}
}