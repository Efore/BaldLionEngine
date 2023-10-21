#include "blpch.h"
#include "PhysicsManager.h"

namespace BaldLion::Physics
{
	reactphysics3d::PhysicsWorld* PhysicsManager::s_world;
	reactphysics3d::PhysicsCommon PhysicsManager::s_physicsCommon;

	float PhysicsManager::s_fixedPhysicsTime;
	bool PhysicsManager::s_physicsActive;
	float PhysicsManager::s_accumulatedTime;


	void PhysicsManager::Init(float fixedPhysicsTime)
	{
		s_physicsActive = true;
		s_fixedPhysicsTime = fixedPhysicsTime;
		s_world = s_physicsCommon.createPhysicsWorld();
	}

	void PhysicsManager::UpdatePhysics(float deltaTime)
	{		
		if (!s_physicsActive)
			return;

		s_accumulatedTime += deltaTime;

		while (s_accumulatedTime >= s_fixedPhysicsTime)
		{
			s_world->update(s_fixedPhysicsTime);
			s_accumulatedTime -= s_fixedPhysicsTime;
		}
	}

	void PhysicsManager::Stop()
	{
		s_physicsCommon.destroyPhysicsWorld(s_world);
	}

	reactphysics3d::RigidBody* PhysicsManager::CreateRigidbody(const glm::vec3& position, const glm::quat& rotation)
	{
		const reactphysics3d::Transform transform(FromGlmVec3(position), FromGlmQuat(rotation));		
		return s_world->createRigidBody(transform);
	}

	void PhysicsManager::DestroyRigidbody(reactphysics3d::RigidBody* rigidbody)
	{
		s_world->destroyRigidBody(rigidbody);
	}

	reactphysics3d::Collider* PhysicsManager::CreateCollider(reactphysics3d::RigidBody* rigidBody, PhysicsShape shape, const glm::vec3& size, const glm::vec3& position, const glm::quat& rotation)
	{
		reactphysics3d::CollisionShape* collissionShape = nullptr;

		switch (shape)
		{
		case BaldLion::Physics::PhysicsShape::Box:
			collissionShape = s_physicsCommon.createBoxShape(FromGlmVec3(size) * 0.5f);
			break;
		case BaldLion::Physics::PhysicsShape::Sphere:
			collissionShape = s_physicsCommon.createSphereShape(size.x);
			break;
		case BaldLion::Physics::PhysicsShape::Capsule:
			collissionShape = s_physicsCommon.createCapsuleShape(size.x, size.y);
			break;
		default:
			break;
		}

		const reactphysics3d::Transform transform(FromGlmVec3(position), FromGlmQuat(rotation));
		return rigidBody->addCollider(collissionShape, transform);
	}

}
