#include "blpch.h"
#include "ECSLocomotionSystem.h"

#include "BaldLion/ECS/ECSManager.h"
#include "BaldLion/ECS/Components/ECSLocomotionComponent.h"
#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/ECS/Components/ECSAnimationComponent.h"

#include "BaldLion/Animation/AnimationManager.h"

#include "BaldLion/Utils/MathUtils.h"
#include <glm/gtx/vector_angle.hpp>


namespace BaldLion
{	
	namespace ECS
	{

		ECSLocomotionSystem::ECSLocomotionSystem(const char* systemName, const ECSSignature& signature, class ECSManager* ecsManager) :
			ECSSystem(systemName, signature, ecsManager, true)
		{

		}

		void ECSLocomotionSystem::UpdateComponents(ECSEntityID entityID, ECSComponentLookUp* componentLookUp, float deltaTime)
		{
			ECSLocomotionComponent* locomotionComponent = componentLookUp->Write<ECSLocomotionComponent>(ECSComponentType::Locomotion);
			ECSTransformComponent* transformComponent = componentLookUp->Write<ECSTransformComponent>(ECSComponentType::Transform);
			
			if (glm::length2(locomotionComponent->desiredVelocity) > FLT_EPSILON * FLT_EPSILON)
			{				
				const glm::mat4 transformMatrix = transformComponent->GetTransformMatrix();
				const glm::vec3 velocity = glm::normalize(locomotionComponent->desiredVelocity);
				const glm::vec3 forward = glm::normalize(MathUtils::GetTransformForwardDirection(transformMatrix));

				const float angle = glm::orientedAngle(forward, velocity, MathUtils::Vector3UnitY) * glm::clamp(deltaTime * locomotionComponent->rotationSpeed, 0.0f, 1.0f);

				const glm::mat4 rotatedMatrix = glm::rotate(transformMatrix, angle, MathUtils::Vector3UnitY);
				MathUtils::DecomposeTransformMatrix(rotatedMatrix, transformComponent->position, transformComponent->rotation, transformComponent->scale);
			}

			const float maxDelta = locomotionComponent->maxAcceleration * deltaTime;
			
			glm::vec3 deltaVelocity = locomotionComponent->desiredVelocity - locomotionComponent->currentVelocity;			
			const float deltaSpeed = glm::length(deltaVelocity);

			if (deltaSpeed > maxDelta)
			{
				const float velocityIncrease = maxDelta / deltaSpeed;
				deltaVelocity *= velocityIncrease;
			}

			locomotionComponent->currentVelocity += deltaVelocity;

			if(glm::length2(locomotionComponent->currentVelocity) > FLT_EPSILON * FLT_EPSILON)
			{
				locomotionComponent->currentMovementSpeed = glm::length(locomotionComponent->currentVelocity);				
				transformComponent->position += locomotionComponent->currentVelocity * deltaTime;
			}
			else 
			{
				locomotionComponent->currentVelocity = MathUtils::Vector3Zero;
				locomotionComponent->currentMovementSpeed = 0.0f;
			}

			ECSAnimationComponent* animationComponent = componentLookUp->Write<ECSAnimationComponent>(ECSComponentType::Animation);
			if (animationComponent != nullptr)
			{
				Animation::AnimatorParameter *parameter = nullptr;
				if (animationComponent->animatorParameters.TryGet(BL_STRING_TO_STRINGID("Speed"), parameter))
				{
					parameter->Value.floating = locomotionComponent->currentMovementSpeed;
				}
			}		

			m_ecsManager->MarkEntityAsChangedInHierarchy(entityID);
		}
	}
}
