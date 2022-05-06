#include "blpch.h"
#include "Animator.h"
#include <glm/gtc/matrix_transform.hpp>

namespace BaldLion
{
	namespace Animation
	{

		Animator::Animator(const std::string& animatorName) : ResourceManagement::Resource(BL_STRING_TO_STRINGID(animatorName), animatorName, ResourceManagement::ResourceType::Animator)
		{
			m_parameters = HashTable<StringId, AnimatorParameter>(Memory::MemoryManager::GetAllocatorType(this), 10);
			m_animations = HashTable<StringId, AnimationData*>(Memory::MemoryManager::GetAllocatorType(this), 15);
			m_transitions = HashTable<StringId, DynamicArray<AnimatorTransition*>>(Memory::MemoryManager::GetAllocatorType(this), 15);
		}

		Animator::~Animator()
		{
			m_animations.Delete();
			m_transitions.Delete();
			m_parameters.Delete();
		}

		void Animator::AddAnimation(AnimationData* animationData)
		{
			if (!m_animations.Contains(animationData->GetResourceID()))
			{
				m_animations.Emplace(animationData->GetResourceID(),std::move(animationData));

				if (m_animations.Size() == 1)
				{
					m_initAnimationID = animationData->GetResourceID();
				}
			}
		}

		const AnimationData* Animator::GetAnimation(const ui32 animationID) const
		{
			AnimationData* animation = nullptr;

			if (m_animations.TryGet(animationID, animation)) {
				return animation;
			}

			return nullptr;
		}

		void Animator::RemoveAnimation(const ui32 animationID)
		{
			if (m_animations.Contains(animationID))
			{
				m_animations.Remove(animationID);
			}
		}

		void Animator::AddAnimationTransition(AnimatorTransition* animationTransition)
		{
			DynamicArray<AnimatorTransition*>* transitions = nullptr;

			if(m_transitions.TryGet(animationTransition->GetInitialAnimationID(), transitions))
			{ 				
				transitions->EmplaceBack(std::move(animationTransition));				
			}
			else 
			{
				DynamicArray<AnimatorTransition*> newTransitions = DynamicArray<AnimatorTransition*>(Memory::MemoryManager::GetAllocatorType(this), 10);
				newTransitions.EmplaceBack(std::move(animationTransition));

				m_transitions.Emplace(animationTransition->GetInitialAnimationID(), std::move(newTransitions));
			}
		}

		void Animator::RemoveTransition(ui32 initialAnimationID, ui32 finalAnimationID)
		{
			DynamicArray<AnimatorTransition*>* transitions = nullptr;

			if (m_transitions.TryGet(initialAnimationID, transitions))
			{
				BL_DYNAMICARRAY_FOR(i, *transitions, 0)
				{
					if ((*transitions)[i]->GetFinalAnimationID() == finalAnimationID)
					{
						(*transitions).RemoveAt(i);
						break;
					}
				}
			}
		}

		const DynamicArray<AnimatorTransition*>* Animator::GetTransitionsOfAnimation(ui32 initialAnimationID) const
		{
			DynamicArray<AnimatorTransition*>* transitions = nullptr;

			if (m_transitions.TryGet(initialAnimationID, transitions))
			{
				return transitions;
			}

			return transitions;
		}

		DynamicArray<AnimatorTransition*>* Animator::GetTransitionsOfAnimation(ui32 initialAnimationID)
		{
			DynamicArray<AnimatorTransition*>* transitions = nullptr;

			if (m_transitions.TryGet(initialAnimationID, transitions))
			{
				return transitions;
			}

			return transitions;
		}

		void Animator::AddParameter(const StringId parameterName, const AnimatorParameter& parameter)
		{
			if (!m_parameters.Contains(parameterName))
			{				
				m_parameters.Emplace(parameterName, std::move(parameter));
			}
		}

		void Animator::RemoveParameter(const StringId parameterName)
		{
			if (m_parameters.Contains(parameterName))
			{
				m_parameters.Remove(parameterName);
			}
		}

		const AnimatorParameter& Animator::GetParameter(const StringId parameterName) const
		{			
			return m_parameters.Get(parameterName);
		}

		void Animator::SetInitialAnimation(const ui32 animationID)
		{
			if (!m_animations.Contains(animationID))
			{
				m_initAnimationID = animationID;
			}
		}

		void Animator::CalculateInterpolatedTransforms(float currentAnimationTime, float currentTransitionTime, const AnimationData* animation, const AnimatorTransition* transition, DynamicArray<JointTransform>& result) const
		{		
			BL_PROFILE_FUNCTION();

			int prevFrameIndex = 0;
			int nextFrameIndex = 0;

			//Finding the correct point in the animation
			BL_DYNAMICARRAY_FOR(i, animation->AnimationFrames, 1)
			{
				if (currentAnimationTime > animation->AnimationFrames[i].TimeStamp)
					continue;

				prevFrameIndex = i - 1;
				nextFrameIndex = i;

				break;
			}

			//Getting the interpolant for that point in the animation
			float interpolant = (currentAnimationTime - animation->AnimationFrames[prevFrameIndex].TimeStamp) / (animation->AnimationFrames[nextFrameIndex].TimeStamp - animation->AnimationFrames[prevFrameIndex].TimeStamp);

			result = DynamicArray<JointTransform>(AllocationType::Linear_Frame, animation->AnimationFrames[prevFrameIndex].JointTranforms);

			//Setting the result as the interpolation between the previous and the next frame to the chosen point in the animation
			BL_DYNAMICARRAY_FOR(i, result, 0)
			{
				result[i].SetPosition(glm::mix(animation->AnimationFrames[prevFrameIndex].JointTranforms[i].GetDecompressedPosition(), animation->AnimationFrames[nextFrameIndex].JointTranforms[i].GetDecompressedPosition(), interpolant));
				result[i].SetRotation(glm::mix(animation->AnimationFrames[prevFrameIndex].JointTranforms[i].GetDecompressedRotation(), animation->AnimationFrames[nextFrameIndex].JointTranforms[i].GetDecompressedRotation(), interpolant));
			}

			//If there is a transition to process
			if (currentTransitionTime >= 0.0f)
			{
				prevFrameIndex = 0;
				nextFrameIndex = 0;

				//Getting the correct point in the final animation
				const AnimationData* nextAnimation = GetAnimation(transition->GetFinalAnimationID());
				BL_DYNAMICARRAY_FOR(i, nextAnimation->AnimationFrames, 1)
				{
					if (currentTransitionTime > nextAnimation->AnimationFrames[i].TimeStamp)
						continue;

					prevFrameIndex = i - 1;
					nextFrameIndex = i;

					break;
				}

				//Getting the interpolant for that point in the final animation
				interpolant = (currentTransitionTime - nextAnimation->AnimationFrames[prevFrameIndex].TimeStamp) / (nextAnimation->AnimationFrames[nextFrameIndex].TimeStamp - nextAnimation->AnimationFrames[prevFrameIndex].TimeStamp);

				DynamicArray<JointTransform> nextAnimationTransforms = DynamicArray<JointTransform>(AllocationType::Linear_Frame, nextAnimation->AnimationFrames[prevFrameIndex].JointTranforms);

				//Setting the transforms for the interpolation between the previous and the next frame to the chosen point in the final animation
				BL_DYNAMICARRAY_FOR(i, nextAnimationTransforms, 0)
				{
					nextAnimationTransforms[i].SetPosition(glm::mix(nextAnimation->AnimationFrames[prevFrameIndex].JointTranforms[i].GetDecompressedPosition(), nextAnimation->AnimationFrames[nextFrameIndex].JointTranforms[i].GetDecompressedPosition(), interpolant));
					nextAnimationTransforms[i].SetRotation(glm::mix(nextAnimation->AnimationFrames[prevFrameIndex].JointTranforms[i].GetDecompressedRotation(), nextAnimation->AnimationFrames[nextFrameIndex].JointTranforms[i].GetDecompressedRotation(), interpolant));
				}

				//Getting the interpolant between both animations
				interpolant = currentTransitionTime / transition->GetTransitionTime();

				//Setting the result
				BL_DYNAMICARRAY_FOR(i, result, 0)
				{
					result[i].SetPosition(glm::mix(result[i].GetDecompressedPosition(), nextAnimationTransforms[i].GetDecompressedPosition(), interpolant));
					result[i].SetRotation(glm::mix(result[i].GetDecompressedRotation(), nextAnimationTransforms[i].GetDecompressedRotation(), interpolant));
				}
			}
		}

		float Animator::GetParameterFloat(const StringId parameterName)
		{
			AnimatorParameter parameter;
			if (m_parameters.TryGet(parameterName, parameter))
			{
				return parameter.Value.floating;
			}

			return -1;
		}

		void Animator::SetParameterFloat(const StringId parameterName, float value)
		{
			AnimatorParameter parameter;
			if (m_parameters.TryGet(parameterName, parameter))
			{
				parameter.Value.floating = value;
			}
		}

		ui32 Animator::GetParameterInt(const StringId parameterName)
		{
			AnimatorParameter parameter;
			if (m_parameters.TryGet(parameterName, parameter))
			{
				return parameter.Value.integer;
			}
			return -1;
		}

		void Animator::SetParameterInt(const StringId parameterName, ui32 value)
		{
			AnimatorParameter parameter;
			if (m_parameters.TryGet(parameterName, parameter))
			{
				parameter.Value.integer = value;
			}
		}

		bool Animator::GetParameterBool(const StringId parameterName)
		{
			AnimatorParameter parameter;
			if (m_parameters.TryGet(parameterName, parameter))
			{
				return parameter.Value.boolean;
			}

			return false;
		}

		void Animator::SetParameterBool(const StringId parameterName, bool value)
		{
			AnimatorParameter parameter;
			if (m_parameters.TryGet(parameterName, parameter))
			{
				parameter.Value.boolean = value;
			}
		}

		const AnimatorTransition* Animator::CheckTransition(const ui32 animationID, float animationTime) const
		{
			DynamicArray<AnimatorTransition*>* transitions = nullptr;

			if (m_transitions.TryGet(animationID, transitions)) {
				
				BL_DYNAMICARRAY_FOR(i, *transitions, 0)
				{
					if ((*transitions)[i]->CheckConditions(*this, animationTime))
						return (*transitions)[i];
				}
			}
			return nullptr;
		}
	}
}

