#pragma once
#include "AnimationData.h"
#include "BaldLion/Rendering/SkinnedMesh.h"
#include "BaldLion/Core/Core.h"
#include "BaldLion/Core/TimeStep.h"
#include <map>

using namespace BaldLion::Rendering;

namespace BaldLion
{
	namespace Animation
	{

		class Animator {
			
		public:
			Animator(SkinnedMesh* animatedMesh, BLVector<AnimationData>& animations, const glm::mat4& rootTransform);
			~Animator();

			void OnUpdate(BaldLion::TimeStep timeStep);
			inline void SetCurrentAnimation(const std::string& animationName);

			void CalculateInterpolatedTransforms(const AnimationData* animation, BLVector<JointTransform>& result);

		private:	

			SkinnedMesh* m_animatedMesh;

			BLVector<AnimationData>* m_animationDataContainer;
			AnimationData* m_currentAnimation;			
			std::unordered_map<std::string,AnimationData*> m_animations;
			glm::mat4 m_rootInverseTransform;
			float m_animationTime;
		};
	}
}