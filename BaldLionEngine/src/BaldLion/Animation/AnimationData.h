#pragma once

#include "Joint.h"
#include "BaldLion/Core/Containers/BLVector.h"

namespace BaldLion
{
	namespace Animation
	{
		struct KeyFrame {
			BLVector<JointTransform> jointTranforms;
			float timeStamp;

			KeyFrame(){}

			KeyFrame(KeyFrame &&other)
			{
				if (jointTranforms.Capacity() > 0)
					jointTranforms.Free();

				timeStamp = other.timeStamp;
				jointTranforms = std::move(other.jointTranforms);
			}

			KeyFrame& operator=(const KeyFrame& other)
			{
				jointTranforms = other.jointTranforms;
				timeStamp = other.timeStamp;

				return *this;
			}

			KeyFrame& operator=(KeyFrame &&other)
			{
				if (jointTranforms.Capacity() > 0)
					jointTranforms.Free();

				timeStamp = other.timeStamp;
				jointTranforms = std::move(other.jointTranforms);

				return *this;
			}
		};

		struct AnimationData {

			AnimationData(){}

			AnimationData(AnimationData&& other)
			{
				if (frames.Capacity() > 0)
					frames.Free();

				animationName = other.animationName;
				animationLength = other.animationLength;
				frames = std::move(other.frames);				
			}

			AnimationData& operator=(const AnimationData& other)
			{
				animationName = other.animationName;
				animationLength = other.animationLength;
				frames = other.frames;

				return *this;
			}

			AnimationData& operator=(AnimationData&& other)
			{
				if (frames.Capacity() > 0)
					frames.Free();

				animationName = other.animationName;
				animationLength = other.animationLength;
				frames = std::move(other.frames);

				return *this;
			}

			std::string animationName;
			BLVector<KeyFrame> frames;
			float animationLength;
		};

	}
}