#pragma once
#include "AnimationData.h"
#include "BaldLion/Rendering/AnimatedModel.h"
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
			Animator(AnimatedModel* animatedModel, const std::vector<Ref<AnimationData>>& animations, const glm::mat4& rootTransform);
			~Animator();

			void OnUpdate(BaldLion::TimeStep timeStep);
			inline void SetCurrentAnimation(const std::string& animationName);

			static std::vector<JointTransform> CalculateInterpolatedTransforms(const Ref<AnimationData>& animation, float animationTime);

		private:	

			AnimatedModel* m_animatedModel;

			Ref<AnimationData> m_currentAnimation;
			std::map<std::string,Ref<AnimationData>> m_animations;
			glm::mat4 m_rootInverseTransform;
			float m_animationTime;
		};
	}
}