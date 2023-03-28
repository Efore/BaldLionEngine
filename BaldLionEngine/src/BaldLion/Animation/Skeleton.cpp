#include "blpch.h"
#include "Skeleton.h"
#include "BaldLion/Utils/StringUtils.h"

namespace BaldLion {

	namespace Animation {


		Skeleton::Skeleton(const AnimationJoint* joints, const std::string& skeletonPath) : 
			ResourceManagement::Resource(BL_STRING_TO_STRINGID(skeletonPath), skeletonPath, ResourceManagement::ResourceType::Skeleton)
		{
			for (ui32 i = 0; i < (ui32)JointType::Count; ++i) 
			{
				m_joints[i] = joints[i];
			}
		}

		Skeleton::~Skeleton()
		{
		}
	}
}