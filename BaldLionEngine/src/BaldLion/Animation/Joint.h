#pragma once

#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "BaldLion/Utils/Quantization.h"

namespace BaldLion
{
	namespace Animation
	{
		#define BL_JOINT_WEIGHTS_PER_VERTEX 3 
		#define BL_JOINT_POSITION_RANGE 100.0F

		struct Joint
		{
		public:			
			uint32_t jointID;
			int32_t parentID;			
			
			glm::mat4 jointBindTransform;
			glm::mat4 jointModelSpaceTransform;
			glm::mat4 jointAnimationTransform;

			void UpdateJointTransforms(const glm::mat4& rootInverseTransform, const glm::mat4& parentTransform, const glm::mat4& localAnimationTransform)
			{				
				jointModelSpaceTransform = parentTransform * localAnimationTransform;
				jointAnimationTransform = rootInverseTransform * jointModelSpaceTransform * jointBindTransform;
			}
		};

		struct JointTransform {	

			Quantization::QuantizedVector3 position;
			Quantization::QuantizedQuaterion rotation;		

			JointTransform()
			{
			}

			JointTransform(const glm::vec3& decompressedPosition, const glm::quat& decompressedRotation)
			{
				position = Quantization::CompressVector3(decompressedPosition, BL_JOINT_POSITION_RANGE);
				rotation = Quantization::CompressQuaternion(decompressedRotation);
			}
			
			glm::vec3 GetDecompressedPosition() 
			{
				return Quantization::DecompressVector3(position, BL_JOINT_POSITION_RANGE);
			}

			void SetPosition(const glm::vec3& decompressedPosition)
			{
				position = Quantization::CompressVector3(decompressedPosition, BL_JOINT_POSITION_RANGE);
			}

			glm::quat GetDecompressedRotation()
			{
				return Quantization::DecompressCuaternion(rotation);
			}

			void SetRotation(const glm::quat& decompressedRotation)
			{
				rotation = Quantization::CompressQuaternion(decompressedRotation);
			}
		};
	}
}