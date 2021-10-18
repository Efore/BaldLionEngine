#pragma once
#include "BaldLion/Core/Containers/DynamicArray.h"
#include "Joint.h"

namespace BaldLion {

	namespace Animation {

		class Skeleton {

		public:
			Skeleton() = default;

			Skeleton(const DynamicArray<Animation::Joint>& joints);

			~Skeleton();

			const DynamicArray<Joint>& GetJoints() const { return m_joints; }
			DynamicArray<Joint>& GetJoints() { return m_joints; }

		private:
			DynamicArray<Joint> m_joints;
		};
	}
}