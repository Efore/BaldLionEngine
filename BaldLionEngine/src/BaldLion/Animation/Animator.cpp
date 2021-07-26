#include "blpch.h"
#include "Animator.h"
#include <glm/gtc/matrix_transform.hpp>

namespace BaldLion
{
	namespace Animation
	{
		Animator::Animator(Skeleton* skeleton, DynamicArray<AnimationData>& animations, const glm::mat4& rootTransform)
			: m_skeleton(skeleton)
		{
			m_animations = HashTable<StringId, AnimationData*>(AllocationType::FreeList_Renderer, animations.Size() * 2);
			for (ui32 i = 0; i < animations.Size(); ++i)
			{
				if (!m_animations.Contains(animations[i].AnimationName))
				{
					m_animations.Emplace(animations[i].AnimationName, &animations[i]);
				}
			}

			m_currentAnimation = &(animations.Back());
			m_rootInverseTransform = glm::inverse(rootTransform);		
			m_animationDataContainer = &animations;
		}

		Animator::~Animator()
		{
			m_animationDataContainer->Clear();
			m_animations.Clear();
		}

		void Animator::CalculateInterpolatedTransforms(const AnimationData* animation, DynamicArray<JointTransform>& result)
		{
			BL_PROFILE_FUNCTION();

			int prevFrameIndex = 0;
			int nextFrameIndex = 0;

			for (ui32 i = 1; i < animation->AnimationFrames.Size(); ++i)
			{
				if (m_animationTime > animation->AnimationFrames[i].TimeStamp)
					continue;

				prevFrameIndex = i - 1;
				nextFrameIndex = i;

				break;
			}

			const float interpolant = (m_animationTime - animation->AnimationFrames[prevFrameIndex].TimeStamp) / (animation->AnimationFrames[nextFrameIndex].TimeStamp - animation->AnimationFrames[prevFrameIndex].TimeStamp) ;

			result = DynamicArray<JointTransform>(AllocationType::Linear_Frame, animation->AnimationFrames[prevFrameIndex].JointTranforms);

			for (ui32 i = 0; i < result.Size(); ++i)
			{
				result[i].SetPosition(glm::mix(animation->AnimationFrames[prevFrameIndex].JointTranforms[i].GetDecompressedPosition(), animation->AnimationFrames[nextFrameIndex].JointTranforms[i].GetDecompressedPosition(), interpolant));
				result[i].SetRotation(glm::mix(animation->AnimationFrames[prevFrameIndex].JointTranforms[i].GetDecompressedRotation(), animation->AnimationFrames[nextFrameIndex].JointTranforms[i].GetDecompressedRotation(), interpolant));				
			}			
		}

		void Animator::OnUpdate(BaldLion::TimeStep timeStep)
		{
			BL_PROFILE_FUNCTION();

			m_animationTime = glm::mod(m_animationTime + timeStep, m_currentAnimation->AnimationLength);

			DynamicArray<JointTransform> transforms;
			CalculateInterpolatedTransforms(m_currentAnimation, transforms); 
			
			for (ui32 i = 0;  i < transforms.Size(); ++i)
			{
				const i32 parentID = m_skeleton->GetJoints()[i].parentID;

				const glm::mat4& parentTransform = parentID == -1 ? glm::mat4(1.0f) : m_skeleton->GetJoints()[parentID].jointModelSpaceTransform;

				const glm::mat4& animationTransform = glm::translate(glm::mat4(1.0f), transforms[i].GetDecompressedPosition()) * glm::mat4_cast(transforms[i].GetDecompressedRotation()) * glm::scale(glm::mat4(1.0f),glm::vec3(1.0f));
				
				m_skeleton->GetJoints()[i].UpdateJointTransforms(m_rootInverseTransform, parentTransform, animationTransform);
			}		

		}

		void Animator::SetCurrentAnimation(StringId AnimationName)
		{
			if(m_animations.Contains(AnimationName))
			{
				m_animationTime = 0;
				m_currentAnimation = m_animations.Get(AnimationName);
			}
		}

	}
}

