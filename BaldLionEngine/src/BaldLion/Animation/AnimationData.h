#pragma once

#include "Joint.h"
#include "BaldLion/Core/Containers/DynamicArray.h"

namespace BaldLion
{
	namespace Animation
	{
		struct KeyFrame {

			DynamicArray<JointTransform> JointTranforms;
			float TimeStamp;

			KeyFrame(){}
			
			KeyFrame(const DynamicArray<JointTransform>& jointTransforms, const float timeStamp): 
				JointTranforms(jointTransforms), 
				TimeStamp(timeStamp)
			{}

			KeyFrame(KeyFrame &&other) : 
				TimeStamp(other.TimeStamp), 
				JointTranforms(std::move(other.JointTranforms))
			{
			}

			KeyFrame& operator=(const KeyFrame& other)
			{
				JointTranforms = other.JointTranforms;
				TimeStamp = other.TimeStamp;

				return *this;
			}

			KeyFrame& operator=(KeyFrame &&other)
			{
				TimeStamp = other.TimeStamp;
				JointTranforms = std::move(other.JointTranforms);				

				return *this;
			}
		};

		struct AnimationData {

			StringId AnimationName;
			DynamicArray<KeyFrame> AnimationFrames;
			float AnimationLength;

			AnimationData(){}

			AnimationData(const StringId animationName, const DynamicArray<KeyFrame>& animationFrames, const float animationLenght) :
				AnimationName(animationName), 
				AnimationFrames(animationFrames), 
				AnimationLength(animationLenght)
			{}

			AnimationData(AnimationData&& other) : 
				AnimationName(other.AnimationName), 
				AnimationLength(other.AnimationLength),
				AnimationFrames(std::move(other.AnimationFrames))
			{
			}

			AnimationData& operator=(const AnimationData& other)
			{
				AnimationName = other.AnimationName;
				AnimationLength = other.AnimationLength;
				AnimationFrames = other.AnimationFrames;

				return *this;
			}

			AnimationData& operator=(AnimationData&& other)
			{
				AnimationName  = other.AnimationName;
				AnimationLength = other.AnimationLength;
				AnimationFrames = std::move(other.AnimationFrames);				

				return *this;
			}			
		};

	}
}