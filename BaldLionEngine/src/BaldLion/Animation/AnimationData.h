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

			KeyFrame(){
			}

			KeyFrame(KeyFrame &&other)
			{
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
				timeStamp = other.timeStamp;
				jointTranforms = std::move(other.jointTranforms);

				return *this;
			}
		};

		struct AnimationData {

			char animationName[1024];
			BLVector<KeyFrame> frames;
			float animationLength;

			AnimationData(){}

			AnimationData(AnimationData&& other)
			{
				strcpy(animationName,other.animationName);				
				animationLength = other.animationLength;
				frames = std::move(other.frames);				
			}

			AnimationData& operator=(const AnimationData& other)
			{
				strcpy(animationName, other.animationName);
				animationLength = other.animationLength;
				frames = other.frames;

				return *this;
			}

			AnimationData& operator=(AnimationData&& other)
			{
				strcpy(animationName, other.animationName);
				animationLength = other.animationLength;
				frames = std::move(other.frames);

				return *this;
			}			
		};

	}
}