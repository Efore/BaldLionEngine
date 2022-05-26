#include "blpch.h"
#include "Animator.h"
#include <glm/gtc/matrix_transform.hpp>

namespace BaldLion
{
	namespace Animation
	{

		Animator::Animator(const std::string& animatorPath) : ResourceManagement::Resource(BL_STRING_TO_STRINGID(animatorPath), animatorPath, ResourceManagement::ResourceType::Animator)
		{
			m_parameters = HashTable<StringId, AnimatorParameter>(Memory::MemoryManager::GetAllocatorType(this), 10);
			m_animations = HashTable<StringId, AnimationClip*>(Memory::MemoryManager::GetAllocatorType(this), 15);
			m_transitions = HashTable<StringId, DynamicArray<AnimatorTransition*>>(Memory::MemoryManager::GetAllocatorType(this), 15);
		}

		Animator::~Animator()
		{
			m_animations.Delete();
			m_transitions.Delete();
			m_parameters.Delete();
		}

		void Animator::AddAnimation(AnimationClip* animationData)
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

		const AnimationClip* Animator::GetAnimation(const ui32 animationID) const
		{
			AnimationClip* animation = nullptr;

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

		void Animator::CalculateInterpolatedTransforms(float currentAnimationTime, float currentTransitionTime, const AnimationClip* animation, const AnimatorTransition* transition, JointTransform* result) const
		{		
			BL_PROFILE_FUNCTION();

			ui32 frame = (ui32)(glm::floor((currentAnimationTime / animation->AnimationTimeLength) * animation->NumFrames));
			ui32 nextFrame = glm::min(animation->NumFrames - 1, frame + 1);

			float frameTime = (animation->AnimationTimeLength / animation->NumFrames) * frame;
			float nextFrameTime = (animation->AnimationTimeLength / animation->NumFrames) * nextFrame;

			float interpolant = (currentAnimationTime - frameTime) / (nextFrameTime - frameTime);

			//Setting the result as the interpolation between the previous and the next frame to the chosen point in the animation
			for (ui32 i = 0; i < (ui32)JointType::Count; ++i)
			{
				if(animation->AnimationTracks[i].TrackStartIndex == -1)
					continue;

				animation->GetFramePosition(i, frame, nextFrame, interpolant, result[i].position);
				animation->GetFrameRotation(i, frame, nextFrame, interpolant, result[i].rotation);
				animation->GetFrameScale(i, frame, nextFrame, interpolant, result[i].scale);
			}
			
			//If there is a transition to process
			if (currentTransitionTime >= 0.0f)
			{
				//Getting the correct point in the final animation
				const AnimationClip* nextAnimation = GetAnimation(transition->GetFinalAnimationID());	

				JointTransform nextAnimationTransforms[(ui32)JointType::Count];

				frame = (ui32)(glm::floor((currentTransitionTime / nextAnimation->AnimationTimeLength) * nextAnimation->NumFrames));
				nextFrame = glm::min(nextAnimation->NumFrames - 1, frame + 1);

				frameTime = (nextAnimation->AnimationTimeLength / nextAnimation->NumFrames) * frame;
				nextFrameTime = (nextAnimation->AnimationTimeLength / nextAnimation->NumFrames) * nextFrame;

				interpolant = (currentTransitionTime - frameTime) / (nextFrameTime - frameTime);

				//Setting the transforms for the interpolation between the previous and the next frame to the chosen point in the final animation
				for (ui32 i = 0; i < (ui32)JointType::Count; ++i)
				{
					if (nextAnimation->AnimationTracks[i].TrackStartIndex == -1)
						continue;

					nextAnimation->GetFramePosition(i, frame, nextFrame, interpolant,  nextAnimationTransforms[i].position);
					nextAnimation->GetFrameRotation(i, frame, nextFrame, interpolant, nextAnimationTransforms[i].rotation);
					nextAnimation->GetFrameScale(i, frame, nextFrame, interpolant, nextAnimationTransforms[i].scale);
				}

				//Getting the interpolant between both animations
				interpolant = currentTransitionTime / transition->GetTransitionTime();

				//Setting the result
				for (ui32 i = 0; i < (ui32)JointType::Count; ++i)
				{
					if (animation->AnimationTracks[i].TrackStartIndex == -1)
						continue;

					result[i].position = (glm::mix(result[i].position, nextAnimationTransforms[i].position, interpolant));
					result[i].rotation = (glm::mix(result[i].rotation, nextAnimationTransforms[i].rotation, interpolant));
					result[i].scale = (glm::mix(result[i].scale, nextAnimationTransforms[i].scale, interpolant));
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

		const AnimatorTransition* Animator::CheckTransition(const ui32 animationID, float animationTime, const HashTable<StringId, AnimatorParameter>& componentParameters) const
		{
			DynamicArray<AnimatorTransition*>* transitions = nullptr;

			if (m_transitions.TryGet(animationID, transitions)) {
				
				BL_DYNAMICARRAY_FOR(i, *transitions, 0)
				{
					if ((*transitions)[i]->CheckConditions(componentParameters, animationTime))
						return (*transitions)[i];
				}
			}
			return nullptr;
		}
	}
}

