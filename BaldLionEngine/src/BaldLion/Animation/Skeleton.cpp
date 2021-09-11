#include "blpch.h"
#include "Skeleton.h"

namespace BaldLion {

	namespace Animation {


		Skeleton::Skeleton(const DynamicArray<Animation::Joint>& joints) : joints(std::move(joints))
		{

		}

		Skeleton::~Skeleton()
		{
			joints.Delete();
		}
	}
}