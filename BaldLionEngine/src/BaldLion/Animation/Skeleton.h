#pragma once
#include "BaldLion/Core/Containers/DynamicArray.h"
#include "Joint.h"
#include "BaldLion/ResourceManagement/Resource.h"

namespace BaldLion {

	namespace Animation {		

		class Skeleton : public ResourceManagement::Resource {

		public:
			Skeleton() = default;

			Skeleton(const Joint* joints, const std::string& resourceName);

			~Skeleton();

			const Joint* GetJoints() const { return m_joints; }
			Joint* GetJoints() { return m_joints; }

		private:
			Joint m_joints[(ui32)JointType::Count];
		};
	}
}