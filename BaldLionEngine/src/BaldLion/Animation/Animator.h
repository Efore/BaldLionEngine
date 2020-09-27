#pragma once
#include "AnimationData.h"
#include "BaldLion/Rendering/AnimatedMesh.h"
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
			Animator(Ref<AnimatedMesh> animatedMesh, const std::vector<Ref<AnimationData>>& animations, const glm::mat4& rootTransform);
			~Animator();

			void OnUpdate(BaldLion::TimeStep timeStep);
			inline void SetCurrentAnimation(const std::string& animationName);

			void CalculateInterpolatedTransforms(const Ref<AnimationData>& animation, std::vector<JointTransform>& result);

		private:	

			Ref<AnimatedMesh> m_animatedMesh;

			Ref<AnimationData> m_currentAnimation;
			std::unordered_map<std::string,Ref<AnimationData>> m_animations;
			glm::mat4 m_rootInverseTransform;
			float m_animationTime;
		};
	}
}