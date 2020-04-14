#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace BaldLion
{
	namespace Animation
	{
		struct Joint
		{
			int jointID;
			int parentJointID;
			std::vector<int> childrenJointIDs;
			glm::mat4 jointTransform;
		};
	}
}