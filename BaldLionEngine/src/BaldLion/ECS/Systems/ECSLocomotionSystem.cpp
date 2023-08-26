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
			
			transformComponent->position += locomotionComponent->desiredVelocity * Time::GetDeltaTime();

			if (glm::length2(locomotionComponent->desiredVelocity) > FLT_EPSILON * FLT_EPSILON)
			{
				const glm::vec3 velocityXZ = glm::normalize(locomotionComponent->desiredVelocity * glm::vec3(1.0f,0.0f,1.0f));

				const glm::quat lookAtQuat = glm::quatLookAt(velocityXZ, MathUtils::Vector3UnitY);

				const float delta = glm::clamp(locomotionComponent->rotationSpeed * Time::GetDeltaTime(), 0.0f, 1.0f);
				const glm::quat newRot = glm::mix(glm::quat(transformComponent->rotation), lookAtQuat, delta);
				
				transformComponent->rotation = glm::eulerAngles(newRot);
			}
		}
	}
}
