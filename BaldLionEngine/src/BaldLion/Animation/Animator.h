#pragma once
#include "AnimationData.h"
#include "BaldLion/Core/Core.h"
#include "BaldLion/Core/TimeStep.h"
#include "BaldLion/Core/Containers/HashTable.h"
#include "Skeleton.h"

namespace BaldLion
{
	namespace Animation
	{

		class Animator {
			
		public:
			Animator(Skeleton* skeleton, DynamicArray<AnimationData>& animations, const glm::mat4& rootTransform);
			~Animator();

			void OnUpdate(BaldLion::TimeStep timeStep);
			inline void SetCurrentAnimation(StringId AnimationName);

			void CalculateInterpolatedTransforms(const AnimationData* animation, DynamicArray<JointTransform>& result);

		private:	

			Skeleton* m_skeleton;

			DynamicArray<AnimationData>* m_animationDataContainer;
			AnimationData* m_currentAnimation;			
			HashTable<StringId,AnimationData*> m_animations;

			glm::mat4 m_rootInverseTransform;

			float m_animationTime;
		};
	}
}