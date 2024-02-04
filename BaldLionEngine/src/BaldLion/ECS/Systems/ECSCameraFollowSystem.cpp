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

		ECSCameraFollowSystem::ECSCameraFollowSystem(const char* systemName, ECSSystemType systemType, const ECSSignature& signature, class ECSManager* ecsManager) : 
			ECSSystem(systemName, systemType, signature, ecsManager, true)
		{

		}

		void ECSCameraFollowSystem::UpdateComponents(ECSEntityID entityID, ECSComponentLookUp* componentLookUp, float deltaTime)
		{
			BL_PROFILE_FUNCTION();

			ECSCameraFollowComponent* cameraFollowComponent = componentLookUp->Write<ECSCameraFollowComponent>(ECSComponentType::CameraFollow);
			ECSTransformComponent* cameraTransformComponent = componentLookUp->Write<ECSTransformComponent>(ECSComponentType::Transform);

			if (cameraFollowComponent->followedEntityID > 0)
			{				
				const ECSTransformComponent* followedEntityTransform = m_ecsManager->GetEntityComponents().Get(cameraFollowComponent->followedEntityID).Read<ECS::ECSTransformComponent>(ECS::ECSComponentType::Transform);
				const glm::mat4 cameraTransformMatrix = cameraTransformComponent->GetTransformMatrix();
				const glm::mat4 followedEntityTransformMatrix = followedEntityTransform->GetTransformMatrix();

				glm::vec4 cameraTargetPosition(0.0f);

				if (m_firstFrame)
				{
					cameraTargetPosition = followedEntityTransformMatrix * glm::vec4(cameraFollowComponent->offsetXY, 0.0f, 1.0f);
					
					cameraFollowComponent->prevEntityFollowedPosition = followedEntityTransform->position;
					cameraFollowComponent->cameraTargetTransform = glm::translate(glm::mat4(1.0f), glm::vec3(cameraTargetPosition)) * glm::toMat4(glm::quat(followedEntityTransform->rotation)) * glm::mat4(1.0f);
				}
				else
				{
					const glm::vec4 followedEntityDeltaPosition = glm::vec4(followedEntityTransform->position - cameraFollowComponent->prevEntityFollowedPosition,1.0f);					
					cameraTargetPosition = cameraFollowComponent->cameraTargetTransform[3] + followedEntityDeltaPosition;
				}

				CalculateInputRotation(cameraFollowComponent, deltaTime);

				//Rotate the camera target around the followed entity
				glm::mat4x4 rotationAroundEntity = glm::rotate(glm::mat4(1.0f), cameraFollowComponent->cameraYaw, MathUtils::Vector3UnitY);

				const glm::vec4 followedEntityPosition(followedEntityTransform->position, 1.0f);
				const glm::vec4 cameraTargetNewPosition = (rotationAroundEntity * glm::vec4(cameraFollowComponent->offsetXY,1.0f,1.0f)) + followedEntityPosition;

				cameraFollowComponent->cameraTargetTransform = glm::translate(glm::mat4(1.0f), glm::vec3(cameraTargetNewPosition)) * rotationAroundEntity * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

				glm::vec4 cameraNewPosition = cameraTargetNewPosition - glm::vec4(MathUtils::GetTransformForwardDirection(cameraFollowComponent->cameraTargetTransform) * cameraFollowComponent->offsetZ,1.0f);

				//Rotate the camera around the pivot point on the world's right axis.
				glm::mat4x4 rotationMatrixY(1.0f);
				rotationMatrixY = glm::rotate(rotationMatrixY, cameraFollowComponent->cameraPitch, MathUtils::GetTransformRightDirection(cameraTransformMatrix));
				cameraNewPosition = (rotationMatrixY * (cameraNewPosition - cameraTargetNewPosition)) + cameraTargetNewPosition;

				cameraTransformComponent->position = cameraNewPosition;

				glm::vec3 directionToFollowedPosition = glm::normalize(glm::vec3(cameraTargetNewPosition) - cameraTransformComponent->position);
				const glm::quat lookAtQuat = glm::quatLookAt(directionToFollowedPosition, MathUtils::Vector3UnitY);

				cameraTransformComponent->rotation = glm::eulerAngles(lookAtQuat);
			}
		}	

		void ECSCameraFollowSystem::CalculateInputRotation(ECSCameraFollowComponent* cameraFollowComponent, float deltaTime)
		{
			glm::vec2 lookInput = Input::InputSystem::GetActionValue(BL_STRING_TO_STRINGID("Look")).value2D;
			float angleX = 0.0f;
			float angleY = 0.0f;

			if (lookInput.x > 1.0f || lookInput.x < -1.0f || lookInput.y > 1.0f || lookInput.y < -1.0f) // mouse adjustment
			{
				angleX = lookInput.x - cameraFollowComponent->prevInputValue.x;
				angleY = -(lookInput.y - cameraFollowComponent->prevInputValue.y);
				cameraFollowComponent->prevInputValue = lookInput;
			}
			else
			{
				angleX = lookInput.x;
				angleY = lookInput.y;
			}

			angleX *= deltaTime * cameraFollowComponent->rotationSpeed;
			angleY *= deltaTime * cameraFollowComponent->rotationSpeed;

			cameraFollowComponent->cameraYaw -= angleX;			

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
