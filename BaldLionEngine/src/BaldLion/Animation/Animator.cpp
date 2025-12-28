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

		const AnimationClip* Animator::GetAnimationClip(const ui32 animationID) const
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
			if (m_animations.Contains(animationID))
			{
				m_initAnimationID = animationID;
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

