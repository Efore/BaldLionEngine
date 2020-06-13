#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace BaldLion
{
	namespace Animation
	{
		#define BL_JOINT_WEIGHTS_PER_VERTEX 3 

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
			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;
		};
	}
}