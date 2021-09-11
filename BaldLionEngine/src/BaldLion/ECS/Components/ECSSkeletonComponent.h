#pragma once
#include "BaldLion/ECS/ECSComponent.h"
#include "BaldLion/Core/Containers/DynamicArray.h"
#include "BaldLion/Animation/Joint.h"

using namespace BaldLion::Animation;

namespace BaldLion {

	namespace ECS
	{
		class ECSSkeletonComponent : public ECSComponent
		{
			ECSSkeletonComponent() :
				ECSComponent(ECSComponentID::Skeleton)
			{

			}

		public:
			DynamicArray<VertexBoneData> boneData;
			DynamicArray<Joint> joints;
		};

	}
}
