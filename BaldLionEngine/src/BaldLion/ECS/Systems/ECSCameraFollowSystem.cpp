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
				const glm::mat4 cameraTransformMatrix = cameraTransformComponent->GetTransformMatrix();

				const ECSTransformComponent* followedEntityTransform = SceneManagement::SceneManager::GetECSManager()->GetEntityComponents().Get(cameraFollowComponent->followedEntityID).Read<ECSTransformComponent>(ECSComponentType::Transform);
				
				const glm::vec3 cameraCenterPosition = followedEntityTransform->position + glm::vec3(cameraFollowComponent->offsetXY,0.0f);
				glm::vec3 cameraDirection = glm::normalize(MathUtils::GetTransformForwardDirection(cameraTransformMatrix));

				cameraTransformComponent->position = cameraCenterPosition - (cameraDirection * cameraFollowComponent->offsetZ);

				glm::vec3 directionToCameraCenter = glm::normalize(cameraCenterPosition - cameraTransformComponent->position);

				float angle = glm::orientedAngle(cameraDirection, directionToCameraCenter, MathUtils::Vector3UnitY);
				glm::mat4 rotatedMatrix = glm::rotate(cameraTransformMatrix, angle, MathUtils::Vector3UnitY);
				
				MathUtils::DecomposeTransformMatrix(rotatedMatrix, cameraTransformComponent->position, cameraTransformComponent->rotation, cameraTransformComponent->scale);

				const float difX = BaldLion::Input::GetMouseX() - cameraFollowComponent->prevX;
				const float difY = BaldLion::Input::GetMouseY() - cameraFollowComponent->prevY;
				const float directionX = difX == 0 ? 0.0f : difX > 0.0f ? 1.0f : -1.0f;
				const float directionY = difY == 0 ? 0.0f : difY > 0.0f ? 1.0f : -1.0f;

				cameraFollowComponent->prevX = BaldLion::Input::GetMouseX();
				cameraFollowComponent->prevY = BaldLion::Input::GetMouseY();

				glm::vec3 cameraOrbitation = glm::normalize(MathUtils::GetTransformRightDirection(cameraTransformMatrix)) * directionX * cameraFollowComponent->rotationSpeed;
				cameraOrbitation += glm::normalize(MathUtils::GetTransformUpDirection(cameraTransformMatrix)) * directionY * cameraFollowComponent->rotationSpeed;
				cameraTransformComponent->position += cameraOrbitation;

				cameraDirection = glm::normalize(MathUtils::GetTransformForwardDirection(cameraTransformMatrix));
				directionToCameraCenter = glm::normalize(cameraCenterPosition - cameraTransformComponent->position);

				angle = glm::orientedAngle(cameraDirection, directionToCameraCenter, MathUtils::Vector3UnitY);
				rotatedMatrix = glm::rotate(cameraTransformMatrix, angle, MathUtils::Vector3UnitY);

				MathUtils::DecomposeTransformMatrix(rotatedMatrix, cameraTransformComponent->position, cameraTransformComponent->rotation, cameraTransformComponent->scale);
			}
		}

	

	}
}
