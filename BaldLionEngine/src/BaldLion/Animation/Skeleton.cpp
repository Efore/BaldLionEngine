#include "blpch.h"
#include "Skeleton.h"
#include "BaldLion/Utils/StringUtils.h"

namespace BaldLion {

	namespace Animation {


		Skeleton::Skeleton(const DynamicArray<Animation::Joint>& joints, const std::string& resourceName) : 
			ResourceManagement::Resource(BL_STRING_TO_STRINGID(resourceName), resourceName, ResourceManagement::ResourceType::Skeleton),
			m_joints(std::move(joints))
		{

		}

		Skeleton::~Skeleton()
		{
			m_joints.Delete();
		}
	}
}