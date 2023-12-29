#include "blpch.h"
#include "ECSPlayerControllerSystem.h"

#include "BaldLion/ECS/ECSManager.h"
#include "BaldLion/Core/Input.h"
#include "BaldLion/Core/KeyCodes.h"
#include "BaldLion/ECS/Components/ECSLocomotionComponent.h"
#include "BaldLion/ECS/ComponentsSingleton/CameraSystem.h"
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
			const ECSTransformComponent* transformComponent = componentLookUp->Read<ECSTransformComponent>(ECSComponentType::Transform);
			ECSLocomotionComponent* locomotionComponent = componentLookUp->Write<ECSLocomotionComponent>(ECSComponentType::Locomotion);
			
			const glm::mat4 cameraTransformMatrix = ECS::SingletonComponents::CameraSystem::GetMainCameraTransform()->GetTransformMatrix();

			glm::vec3 desiredVelocity(0.0f);

			if (BaldLion::Input::IsKeyPressed(BL_KEY_W))
				desiredVelocity -= MathUtils::GetTransformForwardDirection(cameraTransformMatrix);
			else if (BaldLion::Input::IsKeyPressed(BL_KEY_S))
				desiredVelocity += MathUtils::GetTransformForwardDirection(cameraTransformMatrix);

			if (BaldLion::Input::IsKeyPressed(BL_KEY_A))
				desiredVelocity -= MathUtils::GetTransformRightDirection(cameraTransformMatrix);
			else if (BaldLion::Input::IsKeyPressed(BL_KEY_D))
				desiredVelocity += MathUtils::GetTransformRightDirection(cameraTransformMatrix);

			desiredVelocity.y = 0.0f;
			desiredVelocity = glm::normalize(desiredVelocity);

			locomotionComponent->desiredVelocity = desiredVelocity * locomotionComponent->maxMovementSpeed;
		}
	}
}
