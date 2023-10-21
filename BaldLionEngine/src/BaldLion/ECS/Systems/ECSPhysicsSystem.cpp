#include "blpch.h"
#include "ECSPhysicsSystem.h"
#include "BaldLion/Physics/PhysicsManager.h"

#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/ECS/Components/ECSPhysicsBodyComponent.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace BaldLion {

	namespace ECS {

		ECSPhysicsSystem::ECSPhysicsSystem(const char* systemName, const ECSSignature& signature, class ECSManager* ecsManager)
			:ECSSystem(systemName, signature, ecsManager, true, true) 
		{

		}

		ECSPhysicsSystem::~ECSPhysicsSystem()
		{

		}

		void ECSPhysicsSystem::OnUpdate(float deltaTime)
		{
			if (!Physics::PhysicsManager::GetIsPhysicsActive())
				return;

			Physics::PhysicsManager::UpdatePhysics(deltaTime);

			ECSSystem::OnUpdate(deltaTime);
		}

		void ECSPhysicsSystem::UpdateComponents(ECSComponentLookUp* componentLookUp, float deltaTime)
		{
			 ECSTransformComponent* transform = componentLookUp->Write<ECSTransformComponent>(ECSComponentType::Transform);		
			 const ECSPhysicsBodyComponent* physicsBody = componentLookUp->Read<ECSPhysicsBodyComponent>(ECSComponentType::PhysicsBody);

			 reactphysics3d::Transform interpolatedTransform(Physics::FromGlmVec3(transform->position), Physics::FromGlmQuat(glm::quat(transform->rotation)));

			 float factor = Physics::PhysicsManager::GetAccumulatedTime() / Physics::PhysicsManager::GetFixedPhysicsTime();

			 interpolatedTransform = reactphysics3d::Transform::interpolateTransforms(physicsBody->rigidBody->getTransform(), interpolatedTransform, factor);

			 transform->position = Physics::ToGlmVec3(interpolatedTransform.getPosition());
			 transform->rotation = glm::eulerAngles(Physics::ToGlmQuat(interpolatedTransform.getOrientation()));
		}

	}
}
