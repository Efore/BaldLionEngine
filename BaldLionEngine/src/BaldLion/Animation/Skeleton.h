#pragma once
#include "BaldLion/Core/Containers/DynamicArray.h"
#include "Joint.h"
#include "BaldLion/ResourceManagement/Resource.h"

namespace BaldLion {

	namespace Animation {		

		class Skeleton : public ResourceManagement::Resource {

		public:
			Skeleton() = default;

			Skeleton(const AnimationJoint* joints, const std::string& resourceName);

			~Skeleton();

			const AnimationJoint* GetJoints() const { return m_joints; }
			AnimationJoint* GetJoints() { return m_joints; }

		private:
			AnimationJoint m_joints[(ui32)JointType::Count];
		};
	}
}