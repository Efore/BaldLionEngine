#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include "BaldLion/Utils/Compression.h"

namespace BaldLion
{
	namespace Animation
	{

		#define BL_JOINT_WEIGHTS_PER_VERTEX 3 
		#define BL_JOINT_POSITION_RANGE 300.0F

		enum class JointType
		{
			Hips,
			Spine,
			Chest,
			Upper_Chest,
			Neck,
			Head,
			HeadTop_End,
			Left_Shoulder,
			Left_Arm,
			Left_Forearm,
			Left_Hand,
			Left_Hand_Thumb1,
			Left_Hand_Thumb2,
			Left_Hand_Thumb3,
			Left_Hand_Thumb4,
			Left_Hand_Index1,
			Left_Hand_Index2,
			Left_Hand_Index3,
			Left_Hand_Index4,
			Left_Hand_Middle1,
			Left_Hand_Middle2,
			Left_Hand_Middle3,
			Left_Hand_Middle4,
			Left_Hand_Ring1,
			Left_Hand_Ring2,
			Left_Hand_Ring3,
			Left_Hand_Ring4,
			Left_Hand_Pinky1,
			Left_Hand_Pinky2,
			Left_Hand_Pinky3,
			Left_Hand_Pinky4,
			Right_Shoulder,
			Right_Arm,
			Right_Forearm,
			Right_Hand,
			Right_Hand_Thumb1,
			Right_Hand_Thumb2,
			Right_Hand_Thumb3,
			Right_Hand_Thumb4,
			Right_Hand_Index1,
			Right_Hand_Index2,
			Right_Hand_Index3,
			Right_Hand_Index4,
			Right_Hand_Middle1,
			Right_Hand_Middle2,
			Right_Hand_Middle3,
			Right_Hand_Middle4,
			Right_Hand_Ring1,
			Right_Hand_Ring2,
			Right_Hand_Ring3,
			Right_Hand_Ring4,
			Right_Hand_Pinky1,
			Right_Hand_Pinky2,
			Right_Hand_Pinky3,
			Right_Hand_Pinky4,
			Left_Upper_Leg,
			Left_Lower_Leg,
			Left_Foot,
			Left_Foot_Toe_Base,
			Left_Foot_Toe_End,
			Right_Upper_Leg,
			Right_Lower_Leg,
			Right_Foot,
			Right_Foot_Toe_Base,
			Right_Foot_Toe_End,

			Count
		};

		struct Joint
		{
			glm::mat4 jointOffsetTransform;
			glm::mat4 jointLocalSpaceTransform;
			glm::mat4 jointModelSpaceTransform;
			JointType parentJointType;			
			
			void UpdateJointTransforms(const glm::mat4& rootInverseTransform, const glm::mat4& parentTransform, const glm::mat4& localAnimationTransform)
			{	
				jointLocalSpaceTransform = parentTransform * localAnimationTransform;
				jointModelSpaceTransform = rootInverseTransform * jointLocalSpaceTransform * jointOffsetTransform;
			}
		};

		struct JointTransform {	
			
			glm::quat rotation;
			glm::vec3 position;
			glm::vec3 scale;

			JointTransform() {

				rotation = glm::identity<glm::quat>();
				position = glm::vec3(0.0f);
				scale = glm::vec3(1.0f);
			}
		};
	}
}