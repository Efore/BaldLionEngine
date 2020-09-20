#include "blpch.h"
#include "Animator.h"
#include <glm/gtc/matrix_transform.hpp>


namespace BaldLion
{
	namespace Animation
	{
		Animator::Animator(Ref<AnimatedMesh> animatedMesh, const std::vector<Ref<AnimationData>>& animations, const glm::mat4& rootTransform)
			: m_animatedMesh(animatedMesh)
		{
			for (auto animation : animations)
			{
				if (m_animations.find(animation->animationName) == m_animations.end())
				{
					m_animations.emplace(animation->animationName, animation);
				}
				BL_LOG_CORE_INFO("Animator size: {0}", animation->frames.size() * animation->frames[0].jointTranforms.size() * sizeof(Joint));
			}
			m_currentAnimation = animations.back();
			m_rootInverseTransform = glm::inverse(rootTransform);


			
		}

		Animator::~Animator()
		{

		}

		void Animator::CalculateInterpolatedTransforms(const Ref<AnimationData>& animation, std::vector<JointTransform>& result)
		{
			int prevFrameIndex = 0;
			int nextFrameIndex = 0;

			for (int i = 1; i < animation->frames.size(); ++i)
			{
				if (m_animationTime > animation->frames[i].timeStamp)
					continue;

				prevFrameIndex = i - 1;
				nextFrameIndex = i;

				break;
			}

			float interpolant = (m_animationTime - animation->frames[prevFrameIndex].timeStamp) / (animation->frames[nextFrameIndex].timeStamp - animation->frames[prevFrameIndex].timeStamp) ;

			result = std::vector(animation->frames[prevFrameIndex].jointTranforms);

			for (int i = 0; i < result.size(); ++i)
			{
				result[i].SetPosition(glm::mix(animation->frames[prevFrameIndex].jointTranforms[i].GetDecompressedPosition(), animation->frames[nextFrameIndex].jointTranforms[i].GetDecompressedPosition(), interpolant));
				result[i].SetRotation(glm::mix(animation->frames[prevFrameIndex].jointTranforms[i].GetDecompressedRotation(), animation->frames[nextFrameIndex].jointTranforms[i].GetDecompressedRotation(), interpolant));				
			}			
		}

		void Animator::OnUpdate(BaldLion::TimeStep timeStep)
		{
			m_animationTime = glm::mod(m_animationTime + timeStep, m_currentAnimation->animationLength);

			std::vector<JointTransform> transforms;
			CalculateInterpolatedTransforms(m_currentAnimation, transforms); 
			
			for (int i = 0;  i < transforms.size(); ++i)
			{
				int parentID = m_animatedMesh->GetJoints()[i].parentID;

				const glm::mat4& parentTransform = parentID == -1 ? glm::mat4(1.0f) : m_animatedMesh->GetJoints()[parentID].jointModelSpaceTransform;
				const glm::mat4& animationTransform = glm::translate(glm::mat4(1.0f), transforms[i].GetDecompressedPosition()) * glm::mat4_cast(transforms[i].GetDecompressedRotation()) * glm::scale(glm::mat4(1.0f),glm::vec3(1.0f));
				
				m_animatedMesh->GetJoints()[i].UpdateJointTransforms(m_rootInverseTransform, parentTransform, animationTransform);
			}			
		}

		void Animator::SetCurrentAnimation(const std::string& animationName)
		{
			std::map<std::string, Ref<AnimationData>>::const_iterator it = m_animations.find(animationName);

			if(it != m_animations.end())
			{
				m_animationTime = 0;
				m_currentAnimation = it->second;
			}
		}

	}
}

