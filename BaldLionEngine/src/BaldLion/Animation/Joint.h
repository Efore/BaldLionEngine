#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include "BaldLion/Utils/Compression.h"

namespace BaldLion
{
	namespace Animation
	{
		#define BL_JOINT_WEIGHTS_PER_VERTEX 3 
		#define BL_JOINT_POSITION_RANGE 100.0F

		struct Joint
		{
			glm::mat4 jointBindTransform;
			glm::mat4 jointModelSpaceTransform;
			glm::mat4 jointAnimationTransform;

			ui32 jointID;
			i32 parentID;			
			
			void UpdateJointTransforms(const glm::mat4& rootInverseTransform, const glm::mat4& parentTransform, const glm::mat4& localAnimationTransform)
			{			
				BL_PROFILE_FUNCTION();
				jointModelSpaceTransform = parentTransform * localAnimationTransform;
				jointAnimationTransform = rootInverseTransform * jointModelSpaceTransform * jointBindTransform;
			}
		};

		struct JointTransform {	

			Compression::QuantizedVector3 position;
			Compression::QuantizedQuaterion rotation;		

			JointTransform() = default;

			JointTransform(const glm::vec3& decompressedPosition, const glm::quat& decompressedRotation)
			{
				position = Compression::CompressVector3(decompressedPosition, BL_JOINT_POSITION_RANGE);
				rotation = Compression::CompressQuaternion(decompressedRotation);
			}
			
			const glm::vec3 GetDecompressedPosition() const
			{
				BL_PROFILE_FUNCTION();
				return Compression::DecompressVector3(position, BL_JOINT_POSITION_RANGE);
			}

			void SetPosition(const glm::vec3& decompressedPosition)
			{
				position = Compression::CompressVector3(decompressedPosition, BL_JOINT_POSITION_RANGE);
			}

			const glm::quat GetDecompressedRotation() const
			{
				BL_PROFILE_FUNCTION();
				return Compression::DecompressCuaternion(rotation);
			}

			void SetRotation(const glm::quat& decompressedRotation)
			{
				rotation = Compression::CompressQuaternion(decompressedRotation);
			}
		};
	}
}