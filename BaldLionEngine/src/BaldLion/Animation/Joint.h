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
			
			glm::mat4 jointOffsetTransform;
			glm::mat4 jointGlobalTransform;
			glm::mat4 jointAnimationTransform;

			void GenerateAnimationTransform(const glm::mat4& rootInverseTransform)
			{				
				jointAnimationTransform = rootInverseTransform * jointGlobalTransform * jointOffsetTransform;
			}
		};

		struct JointTransform {		
			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;
		};
	}
}