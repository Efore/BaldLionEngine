#pragma once
#include "Joint.h"
#include <vector>

namespace BaldLion
{
	namespace Animation
	{
		struct KeyFrame {
			std::vector<JointTransform> jointTranforms;
			float timeStamp;
		};

		struct AnimationData {

			std::string animationName;
			std::vector<KeyFrame> frames;
			float animationLength;			
			float currentAnimationtime;		

			void SetCurrentAnimationTime(const float timeStep) {

				currentAnimationtime += timeStep;

				if (currentAnimationtime > animationLength)
					currentAnimationtime = currentAnimationtime - animationLength;
			}

		};

	}
}