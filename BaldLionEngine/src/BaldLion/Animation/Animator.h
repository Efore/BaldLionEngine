#pragma once
#include "AnimationData.h"
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
			Animator(const std::string& animatorName);
			~Animator();

			void AddAnimation(AnimationData* animationData);
			const AnimationData* GetAnimation(const ui32 animationID) const;		
			void RemoveAnimation(const ui32 animationID);

			const ui32 GetInitialAnimationID() const { return m_initAnimationID; }
			void SetInitialAnimation(const ui32 animationID);

			const HashTable<ui32, AnimationData*>& GetAllAnimations() const { return m_animations; }

			void CalculateInterpolatedTransforms(float currentAnimationTime, float currentTransitionTime, const AnimationData* animation, const AnimatorTransition* transition, DynamicArray<JointTransform>& result) const;

			void AddAnimationTransition(AnimatorTransition* animationTransition);		
			void RemoveTransition(ui32 initialAnimationID, ui32 finalAnimationID);

			const DynamicArray<AnimatorTransition*>* GetTransitionsOfAnimation(ui32 initialAnimationID) const;
			DynamicArray<AnimatorTransition*>* GetTransitionsOfAnimation(ui32 initialAnimationID);
			const HashTable<ui32, DynamicArray<AnimatorTransition*>>& GetAllTransitions() const { return m_transitions; }

			const HashTable<StringId, AnimatorParameter>& GetAllParameters() const { return m_parameters; }

			void AddParameter(const StringId parameterName, const AnimatorParameter& parameter);
			void RemoveParameter(const StringId parameterName);

			const AnimatorParameter& GetParameter(const StringId parameterName) const;

			float GetParameterFloat(const StringId parameterName);
			void SetParameterFloat(const StringId parameterName, float value);

			ui32 GetParameterInt(const StringId parameterName);
			void SetParameterInt(const StringId parameterName, ui32 value);

			bool GetParameterBool(const StringId parameterName);
			void SetParameterBool(const StringId parameterName, bool value);

			const AnimatorTransition* CheckTransition(const ui32 animationID, float animationTime) const;

		private:	

			HashTable<ui32, AnimationData*> m_animations;
			HashTable<ui32, DynamicArray<AnimatorTransition*>> m_transitions;
			HashTable<StringId, AnimatorParameter> m_parameters;

			ui32 m_initAnimationID;
		};
	}
}