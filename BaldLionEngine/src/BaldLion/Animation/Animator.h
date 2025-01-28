#pragma once
#include "AnimationClip.h"
#include "BaldLion/Core/Core.h"
#include "BaldLion/Core/Containers/HashTable.h"
#include "Joint.h"
#include "AnimatorTransition.h"
#include "BaldLion/ResourceManagement/Resource.h"

namespace BaldLion
{
	namespace Animation
	{
		class Animator : public ResourceManagement::Resource {			

		public:
			Animator() = default;
			Animator(const std::string& animatorPath);
			~Animator();

			void AddAnimation(AnimationClip* animationData);
			const AnimationClip* GetAnimationClip(const ui32 animationID) const;		
			void RemoveAnimation(const ui32 animationID);

			const ui32 GetInitialAnimationID() const { return m_initAnimationID; }
			void SetInitialAnimation(const ui32 animationID);

			const HashTable<StringId, AnimationClip*>& GetAllAnimations() const { return m_animations; }

			void AddAnimationTransition(AnimatorTransition* animationTransition);		
			void RemoveTransition(ui32 initialAnimationID, ui32 finalAnimationID);

			const DynamicArray<AnimatorTransition*>* GetTransitionsOfAnimation(ui32 initialAnimationID) const;
			DynamicArray<AnimatorTransition*>* GetTransitionsOfAnimation(ui32 initialAnimationID);
			const HashTable<StringId, DynamicArray<AnimatorTransition*>>& GetAllTransitions() const { return m_transitions; }

			const HashTable<StringId, AnimatorParameter>& GetAllParameters() const { return m_parameters; }

			void AddParameter(const StringId parameterName, const AnimatorParameter& parameter);
			void RemoveParameter(const StringId parameterName);

			const AnimatorParameter& GetParameter(const StringId parameterName) const;

			const AnimatorTransition* CheckTransition(const ui32 animationID, float animationTime, const HashTable<StringId, AnimatorParameter>& componentParameters) const;

		private:	

			HashTable<StringId, AnimationClip*> m_animations;
			HashTable<StringId, DynamicArray<AnimatorTransition*>> m_transitions;
			HashTable<StringId, AnimatorParameter> m_parameters;

			ui32 m_initAnimationID;
		};
	}
}