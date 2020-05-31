#include "blpch.h"
#include "Animator.h"
#include <glm/gtc/matrix_transform.hpp>


namespace BaldLion
{
	namespace Animation
	{
		Animator::Animator(AnimatedModel* animatedModel, const std::vector<Ref<AnimationData>>& animations, const glm::mat4& rootTransform)
			: m_animatedModel(animatedModel)
		{
			for (auto animation : animations)
			{
				if (m_animations.find(animation->animationName) == m_animations.end())
				{
					m_animations.emplace(animation->animationName, animation);
				}
			}
			m_currentAnimation = animations.back();
			m_rootInverseTransform = glm::inverse(rootTransform);
		}

		Animator::~Animator()
		{

		}

		std::vector<JointTransform> Animator::CalculateInterpolatedTransforms(const Ref<AnimationData>& animation, float animationTime)
		{
			int prevFrameIndex = 0;
			int nextFrameIndex = 0;

			for (int i = 1; i < animation->frames.size(); ++i)
			{
				if (animationTime > animation->frames[i].timeStamp)
					continue;

				prevFrameIndex = i - 1;
				nextFrameIndex = i;

				break;
			}

			float interpolant = (animationTime - animation->frames[prevFrameIndex].timeStamp) / (animation->frames[nextFrameIndex].timeStamp - animation->frames[prevFrameIndex].timeStamp) ;

			std::vector<JointTransform> result = std::vector(animation->frames[prevFrameIndex].jointTranforms);

			for (int i = 0; i < result.size(); ++i)
			{
				result[i].position = glm::mix(animation->frames[prevFrameIndex].jointTranforms[i].position, animation->frames[nextFrameIndex].jointTranforms[i].position, interpolant);
				result[i].rotation = glm::mix(animation->frames[prevFrameIndex].jointTranforms[i].rotation, animation->frames[nextFrameIndex].jointTranforms[i].rotation, interpolant);
				result[i].scale = glm::mix(animation->frames[prevFrameIndex].jointTranforms[i].scale, animation->frames[nextFrameIndex].jointTranforms[i].scale, interpolant);
			}

			return result;
		}

		void Animator::OnUpdate(BaldLion::TimeStep timeStep)
		{
			m_currentAnimation->currentAnimationtime += timeStep;

			if (m_currentAnimation->currentAnimationtime > m_currentAnimation->animationLength)
				m_currentAnimation->currentAnimationtime = m_currentAnimation->currentAnimationtime - m_currentAnimation->animationLength;

			const std::vector<JointTransform> transforms = Animator::CalculateInterpolatedTransforms(m_currentAnimation, m_currentAnimation->currentAnimationtime);
			
			for (int i = 0;  i < transforms.size(); ++i)
			{
				int parentID = m_animatedModel->GetSubMeshes()[0].GetJoints()[i].parentID;

				glm::mat4 parentTransform = parentID == -1 ? glm::mat4(1.0f) : m_animatedModel->GetSubMeshes()[0].GetJoints()[parentID].globalTransform;
				glm::mat4 animationTransform = glm::translate(glm::mat4(1.0f), transforms[i].position) *  glm::mat4_cast(transforms[i].rotation) * glm::scale(glm::mat4(1.0f), transforms[i].scale);

				m_animatedModel->GetSubMeshes()[0].GetJoints()[i].globalTransform = parentTransform * animationTransform;
				m_animatedModel->GetSubMeshes()[0].GetJoints()[i].GenerateAnimationTransform(m_rootInverseTransform);
			}			
		}

		void Animator::SetCurrentAnimation(const std::string& animationName)
		{
			std::map<std::string, Ref<AnimationData>>::const_iterator it = m_animations.find(animationName);

			if(it != m_animations.end())
			{
				m_currentAnimation = it->second;
			}
		}

	}
}

