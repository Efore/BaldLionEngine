#include "blpch.h"
#include "ECSLocomotionSystem.h"

#include "BaldLion/ECS/Components/ECSLocomotionComponent.h"
#include "BaldLion/ECS/Components/ECSTransformComponent.h"

#include "BaldLion/Utils/MathUtils.h"
#include <glm/gtx/vector_angle.hpp>


namespace BaldLion
{	
	namespace ECS
	{

		ECSLocomotionSystem::ECSLocomotionSystem(const char* systemName, const ECSSignature& signature, class ECSManager* ecsManager) :
			ECSSystem(systemName, signature, ecsManager, true, true)
		{

		}

		void ECSLocomotionSystem::UpdateComponents(ECSComponentLookUp* componentLookUp)
		{
			const ECSLocomotionComponent* locomotionComponent = componentLookUp->Read<ECSLocomotionComponent>(ECSComponentType::Locomotion);
			ECSTransformComponent* transformComponent = componentLookUp->Write<ECSTransformComponent>(ECSComponentType::Transform);
			
			transformComponent->position = locomotionComponent->nextPosition;

			if (glm::length2(locomotionComponent->desiredVelocity) > FLT_EPSILON * FLT_EPSILON)
			{				
				const glm::mat4 transformMatrix = transformComponent->GetTransformMatrix();
				const glm::vec3 velocity = glm::normalize(locomotionComponent->desiredVelocity);
				const glm::vec3 forward = glm::normalize(MathUtils::GetTransformForwardDirection(transformMatrix));

				const float angle = glm::orientedAngle(forward, velocity, MathUtils::Vector3UnitY) * glm::clamp(Time::GetDeltaTime() * locomotionComponent->rotationSpeed, 0.0f, 1.0f);

				const glm::mat4 rotatedMatrix = glm::rotate(transformMatrix, angle, MathUtils::Vector3UnitY);
				MathUtils::DecomposeTransformMatrix(rotatedMatrix, transformComponent->position, transformComponent->rotation, transformComponent->scale);
			}
		}
	}
}
