#pragma once
#include "BaldLion/ECS/ECSComponent.h"
#include "BaldLion/Core/Containers/DynamicArray.h"
#include "BaldLion/Animation/Joint.h"
#include "BaldLion/Rendering/Vertex.h"

using namespace BaldLion::Animation;

namespace BaldLion {

	namespace ECS
	{
		class ECSSkeletonComponent : public ECSComponent
		{
		public:

			ECSSkeletonComponent() :
				ECSComponent(ECSComponentType::Skeleton)
			{

			}

			~ECSSkeletonComponent() {
				boneData.Delete();
				joints.Delete();
			}

		public:
			DynamicArray<Rendering::VertexBone> boneData;
			DynamicArray<Joint> joints;
			ui32 skeletonResourceID;
		};

	}
}
