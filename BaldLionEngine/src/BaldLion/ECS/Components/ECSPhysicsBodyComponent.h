#pragma once
#include "BaldLion/ECS/ECSComponent.h"
#include "BaldLion/Physics/PhysicsManager.h"


namespace BaldLion {

	namespace ECS
	{
		class ECSPhysicsBodyComponent : public ECSComponent
		{

		public:
			ECSPhysicsBodyComponent(Physics::PhysicsShape shape, const glm::vec3& colliderSize,  const glm::vec3& position, const glm::vec3& rotation, float initialMass) :
				ECSComponent(ECSComponentType::PhysicsBody),
				shape(shape),
				bodyType(Physics::PhysicsBodyType::Static),
				unscaledColliderSize(colliderSize)

			{
				rigidBody = Physics::PhysicsManager::CreateRigidbody(position, glm::quat(rotation));
				rigidBody->setMass(initialMass);
				rigidBody->setType(reactphysics3d::BodyType::STATIC);
				collider = Physics::PhysicsManager::CreateCollider(rigidBody, shape, colliderSize, glm::vec3(0.0f), glm::quat(glm::vec3(0.0f)));
			}

			~ECSPhysicsBodyComponent() {
				Physics::PhysicsManager::DestroyRigidbody(rigidBody);
				rigidBody = nullptr;
				collider = nullptr;
			}

		public:

			Physics::PhysicsShape shape;
			Physics::PhysicsBodyType bodyType;
			glm::vec3 unscaledColliderSize;
			reactphysics3d::RigidBody*	rigidBody;
			reactphysics3d::Collider*	collider;
		};

	}
}

