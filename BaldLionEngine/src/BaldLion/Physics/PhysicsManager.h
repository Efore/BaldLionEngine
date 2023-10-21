#pragma once
#include <ReactPhysics3D/reactphysics3d.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


namespace BaldLion::Physics
{
	enum class PhysicsShape {
		Box,
		Sphere,
		Capsule
	};

	enum class PhysicsBodyType {
		Dynamic,
		Static,
		Kinematic
	};

	class PhysicsManager {

	public:
		static void Init(float fixedPhysicsTime);
		static void UpdatePhysics(float deltaTime);
		static void Stop();

		static float GetAccumulatedTime() { return s_accumulatedTime; }
		static float GetFixedPhysicsTime() { return s_fixedPhysicsTime; }

		static void SetIsPhysicsActive(bool update) { s_physicsActive = update; }
		static bool GetIsPhysicsActive() { return s_physicsActive; }

		static reactphysics3d::Collider* CreateCollider(reactphysics3d::RigidBody*  rigidBody, PhysicsShape shape, const glm::vec3& size, const glm::vec3& position, const glm::quat& rotation);

		static reactphysics3d::RigidBody* CreateRigidbody(const glm::vec3& position, const glm::quat& rotation);
		static void DestroyRigidbody(reactphysics3d::RigidBody* rigidbody);

	private:

		static bool s_physicsActive;
		static float s_fixedPhysicsTime;
		static float s_accumulatedTime;
		static reactphysics3d::PhysicsCommon s_physicsCommon;
		static reactphysics3d::PhysicsWorld* s_world;
	};

	__forceinline glm::vec3 ToGlmVec3(const reactphysics3d::Vector3& vec3) {
		return glm::vec3(vec3.x, vec3.y, vec3.z);
	}

	__forceinline reactphysics3d::Vector3 FromGlmVec3(const glm::vec3& vec3) {
		return  reactphysics3d::Vector3(vec3.x, vec3.y, vec3.z);
	}

	__forceinline glm::quat ToGlmQuat(const reactphysics3d::Quaternion& quat) {
		return  glm::quat(quat.w, quat.x, quat.y, quat.z);
	}

	__forceinline reactphysics3d::Quaternion FromGlmQuat(const glm::quat& quat) {
		return  reactphysics3d::Quaternion(quat.x, quat.y, quat.z, quat.w);
	}
}
