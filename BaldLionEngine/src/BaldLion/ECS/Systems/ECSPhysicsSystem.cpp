#include "blpch.h"
#include "ECSPhysicsSystem.h"
#include "BaldLion/Physics/PhysicsManager.h"

#include "BaldLion/ECS/ECSManager.h"
#include "BaldLion/ECS/Components/ECSTransformComponent.h"
#include "BaldLion/ECS/Components/ECSPhysicsBodyComponent.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/epsilon.hpp>

namespace BaldLion {

	namespace ECS {

		ECSPhysicsSystem::ECSPhysicsSystem(const char* systemName, const ECSSignature& signature, class ECSManager* ecsManager)
			:ECSSystem(systemName, signature, ecsManager, true) 
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

		void ECSPhysicsSystem::UpdateComponents(ECSEntityID entityID, ECSComponentLookUp* componentLookUp, float deltaTime)
		{
			 ECSTransformComponent* transform = componentLookUp->Write<ECSTransformComponent>(ECSComponentType::Transform);		
			 ECSPhysicsBodyComponent* physicsBody = componentLookUp->Write<ECSPhysicsBodyComponent>(ECSComponentType::PhysicsBody);

			 switch (physicsBody->shape)
			 {

			 case Physics::PhysicsShape::Box:
			 {
				 glm::vec3 boxSize = Physics::ToGlmVec3(((reactphysics3d::BoxShape*)physicsBody->collider->getCollisionShape())->getHalfExtents()) * 2.0f;

				 if (glm::any(glm::epsilonNotEqual((physicsBody->unscaledColliderSize * transform->scale), boxSize, FLT_EPSILON)))
				 {
					 boxSize = physicsBody->unscaledColliderSize * transform->scale;
					 ((reactphysics3d::BoxShape*)physicsBody->collider->getCollisionShape())->setHalfExtents(Physics::FromGlmVec3(boxSize * 0.5f));
				 }
			 }
				 break;

			 case Physics::PhysicsShape::Sphere:
			 {
				 float radius = ((reactphysics3d::SphereShape*)physicsBody->collider->getCollisionShape())->getRadius();

				 if (glm::epsilonNotEqual(physicsBody->unscaledColliderSize.x * transform->scale.x, radius, FLT_EPSILON))
				 {
					 radius = physicsBody->unscaledColliderSize.x * transform->scale.x;
					 ((reactphysics3d::SphereShape*)physicsBody->collider->getCollisionShape())->setRadius(radius);
				 }
			 }
				 break;

			 case Physics::PhysicsShape::Capsule:
			 {
				 float radius = ((reactphysics3d::CapsuleShape*)physicsBody->collider->getCollisionShape())->getRadius();
				 float height = ((reactphysics3d::CapsuleShape*)physicsBody->collider->getCollisionShape())->getHeight();

				 if (glm::epsilonNotEqual(physicsBody->unscaledColliderSize.x * transform->scale.x, radius, FLT_EPSILON) ||
					 glm::epsilonNotEqual(physicsBody->unscaledColliderSize.y * transform->scale.y, height, FLT_EPSILON))
				 {
					 radius = physicsBody->unscaledColliderSize.x * transform->scale.x;
					 height = physicsBody->unscaledColliderSize.y * transform->scale.y;
					 ((reactphysics3d::CapsuleShape*)physicsBody->collider->getCollisionShape())->setRadius(radius);
					 ((reactphysics3d::CapsuleShape*)physicsBody->collider->getCollisionShape())->setHeight(height);
				 }
			 }
				 break;
			 }



			 reactphysics3d::Transform interpolatedTransform(Physics::FromGlmVec3(transform->position), Physics::FromGlmQuat(glm::quat(transform->rotation)));

			 float factor = Physics::PhysicsManager::GetAccumulatedTime() / Physics::PhysicsManager::GetFixedPhysicsTime();

			 interpolatedTransform = reactphysics3d::Transform::interpolateTransforms(physicsBody->rigidBody->getTransform(), interpolatedTransform, factor);

			 transform->position = Physics::ToGlmVec3(interpolatedTransform.getPosition());
			 transform->rotation = glm::eulerAngles(Physics::ToGlmQuat(interpolatedTransform.getOrientation()));

			 m_ecsManager->MarkEntityAsChangedInHierarchy(entityID);
		}

	}
}
