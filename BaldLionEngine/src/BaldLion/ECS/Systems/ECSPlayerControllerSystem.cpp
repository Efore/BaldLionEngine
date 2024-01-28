#include "blpch.h"
#include "ECSPlayerControllerSystem.h"

#include "BaldLion/ECS/ECSManager.h"
#include "BaldLion/Core/Input.h"
#include "BaldLion/Core/BLKeyCodes.h"
#include "BaldLion/ECS/Components/ECSLocomotionComponent.h"
#include "BaldLion/ECS/SingletonSystems/CameraSystem.h"
#include "BaldLion/ECS/Components/ECSPlayerControllerComponent.h"
#include "BaldLion/Utils/MathUtils.h"


namespace BaldLion
{	
	namespace ECS
	{
		ECSPlayerControllerSystem::ECSPlayerControllerSystem(const char* systemName, ECSSystemType systemType, const ECSSignature& signature, class ECSManager* ecsManager) :
			ECSSystem(systemName, systemType, signature, ecsManager, false)
		{

		}

		void ECSPlayerControllerSystem::UpdateComponents(ECSEntityID entityID, ECSComponentLookUp* componentLookUp, float deltaTime)
		{
			BL_PROFILE_FUNCTION();

			const ECSTransformComponent* transformComponent = componentLookUp->Read<ECSTransformComponent>(ECSComponentType::Transform);
			ECSLocomotionComponent* locomotionComponent = componentLookUp->Write<ECSLocomotionComponent>(ECSComponentType::Locomotion);
			
			const glm::mat4 cameraTransformMatrix = ECS::SingletonSystems::CameraSystem::GetMainCameraTransform()->GetTransformMatrix();

			glm::vec3 desiredVelocity(0.0f);

			glm::vec2 moveInput = Input::InputSystem::GetActionValue(BL_STRING_TO_STRINGID("Move")).value2D;

			desiredVelocity -= MathUtils::GetTransformForwardDirection(cameraTransformMatrix) * moveInput.y;
			desiredVelocity += MathUtils::GetTransformRightDirection(cameraTransformMatrix) * moveInput.x;

			desiredVelocity.y = 0.0f;
			desiredVelocity = glm::normalize(desiredVelocity);

			locomotionComponent->desiredVelocity = desiredVelocity * locomotionComponent->maxMovementSpeed;
		}
	}
}
