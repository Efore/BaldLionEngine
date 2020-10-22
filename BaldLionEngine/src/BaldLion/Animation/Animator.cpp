#include "blpch.h"
#include "Animator.h"
#include <glm/gtc/matrix_transform.hpp>


namespace BaldLion
{
	namespace Animation
	{
		Animator::Animator(SkinnedMesh* animatedMesh, BLVector<AnimationData>& animations, const glm::mat4& rootTransform)
			: m_animatedMesh(animatedMesh)
		{
			for (size_t i = 0; i < animations.Size(); ++i)
			{
				if (m_animations.find(animations[i].animationName) == m_animations.end())
				{
					m_animations.emplace(animations[i].animationName, &animations[i]);
				}
			}

			m_currentAnimation = &(animations.Back());
			m_rootInverseTransform = glm::inverse(rootTransform);		
			m_animationDataContainer = &animations;
		}

		Animator::~Animator()
		{
			m_animationDataContainer->Free();
		}

		void Animator::CalculateInterpolatedTransforms(const AnimationData* animation, BLVector<JointTransform>& result)
		{
			int prevFrameIndex = 0;
			int nextFrameIndex = 0;

			for (int i = 1; i < animation->frames.Size(); ++i)
			{
				if (m_animationTime > animation->frames[i].timeStamp)
					continue;

				prevFrameIndex = i - 1;
				nextFrameIndex = i;

				break;
			}

			float interpolant = (m_animationTime - animation->frames[prevFrameIndex].timeStamp) / (animation->frames[nextFrameIndex].timeStamp - animation->frames[prevFrameIndex].timeStamp) ;

			result = BLVector<JointTransform>(AllocationType::Linear_Frame, animation->frames[prevFrameIndex].jointTranforms);

			for (size_t i = 0; i < result.Size(); ++i)
			{
				result[i].SetPosition(glm::mix(animation->frames[prevFrameIndex].jointTranforms[i].GetDecompressedPosition(), animation->frames[nextFrameIndex].jointTranforms[i].GetDecompressedPosition(), interpolant));
				result[i].SetRotation(glm::mix(animation->frames[prevFrameIndex].jointTranforms[i].GetDecompressedRotation(), animation->frames[nextFrameIndex].jointTranforms[i].GetDecompressedRotation(), interpolant));				
			}			
		}

		void Animator::OnUpdate(BaldLion::TimeStep timeStep)
		{
			m_animationTime = glm::mod(m_animationTime + timeStep, m_currentAnimation->animationLength);

			BLVector<JointTransform> transforms;
			CalculateInterpolatedTransforms(m_currentAnimation, transforms); 
			
			for (int i = 0;  i < transforms.Size(); ++i)
			{
				int parentID = m_animatedMesh->GetJoints()[i].parentID;

				const glm::mat4& parentTransform = parentID == -1 ? glm::mat4(1.0f) : m_animatedMesh->GetJoints()[parentID].jointModelSpaceTransform;
				const glm::mat4& animationTransform = glm::translate(glm::mat4(1.0f), transforms[i].GetDecompressedPosition()) * glm::mat4_cast(transforms[i].GetDecompressedRotation()) * glm::scale(glm::mat4(1.0f),glm::vec3(1.0f));
				
				m_animatedMesh->GetJoints()[i].UpdateJointTransforms(m_rootInverseTransform, parentTransform, animationTransform);
			}			
		}

		void Animator::SetCurrentAnimation(const std::string& animationName)
		{
			std::unordered_map<std::string,AnimationData*>::const_iterator it = m_animations.find(animationName);

			if(it != m_animations.end())
			{
				m_animationTime = 0;
				m_currentAnimation = it->second;
			}
		}

	}
}

