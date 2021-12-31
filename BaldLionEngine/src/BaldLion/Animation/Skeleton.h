#pragma once
#include "BaldLion/Core/Containers/DynamicArray.h"
#include "Joint.h"
#include "BaldLion/ResourceManagement/Resource.h"

namespace BaldLion {

	namespace Animation {

		class Skeleton : public ResourceManagement::Resource {

		public:
			Skeleton() = default;

			Skeleton(const DynamicArray<Animation::Joint>& joints, const std::string& resourceName);

			~Skeleton();

			const DynamicArray<Joint>& GetJoints() const { return m_joints; }
			DynamicArray<Joint>& GetJoints() { return m_joints; }

		private:
			DynamicArray<Joint> m_joints;
		};
	}
}