#include "blpch.h"
#include "Skeleton.h"

namespace BaldLion {

	namespace Animation {


		Skeleton::Skeleton(const DynamicArray<Animation::Joint>& joints) : m_joints(std::move(joints))
		{

		}

		Skeleton::~Skeleton()
		{
			m_joints.Clear();
		}
	}
}