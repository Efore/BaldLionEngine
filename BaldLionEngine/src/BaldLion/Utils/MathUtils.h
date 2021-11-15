#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <assimp/scene.h>

namespace BaldLion
{
	namespace MathUtils {

		static glm::mat4 AiMat4ToGlmMat4(const aiMatrix4x4& aiMat4)
		{
			return glm::mat4(
				aiMat4.a1, aiMat4.b1, aiMat4.c1, aiMat4.d1,
				aiMat4.a2, aiMat4.b2, aiMat4.c2, aiMat4.d2,
				aiMat4.a3, aiMat4.b3, aiMat4.c3, aiMat4.d3,
				aiMat4.a4, aiMat4.b4, aiMat4.c4, aiMat4.d4
			);
		}

		static glm::vec3 Vector3UnitX	= glm::vec3(1.0f, 0.0f, 0.0f);
		static glm::vec3 Vector3UnitY	= glm::vec3(0.0f, 1.0f, 0.0f);
		static glm::vec3 Vector3UnitZ	= glm::vec3(0.0f, 0.0f, 1.0f);
		static glm::vec3 Vector3Zero	= glm::vec3(0.0f);

		static glm::quat QuaternionIdentity = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

		static glm::vec3 GetTransformForwardDirection(const glm::mat4& transformMatrix) {

			return glm::vec3(transformMatrix[2][0], transformMatrix[2][1], transformMatrix[2][2]);
		}

		static glm::vec3 GetTransformRightDirection(const glm::mat4& transformMatrix) {

			return glm::vec3(transformMatrix[0][0], transformMatrix[0][1], transformMatrix[0][2]);
		}

	} 
}