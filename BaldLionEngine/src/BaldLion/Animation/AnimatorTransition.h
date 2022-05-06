#pragma once
#include <functional>
#include "AnimationUtils.h"

namespace BaldLion
{
	namespace Animation
	{
		class AnimatorTransition
		{

		public:

			AnimatorTransition();
			AnimatorTransition(ui32 initialAnimationID, ui32 finalAnimationID, float exitTime, float transitionTime);
			AnimatorTransition(AnimatorTransition&& other);

			~AnimatorTransition();

			AnimatorTransition& operator=(AnimatorTransition&& other);
			AnimatorTransition& operator=(const AnimatorTransition& other);

			bool operator== (const AnimatorTransition& other);
			bool operator!= (const AnimatorTransition& other);

			void AddCondition(const AnimatorCondition& condition);
			bool CheckConditions(const class Animator& animator, float animationTime) const;
			const DynamicArray<AnimatorCondition>& GetConditions() const { return m_conditions; }
			void RemoveCondition(ui32 conditionIndex);

			ui32 GetInitialAnimationID() const { return m_initialAnimationID; }
			ui32 GetFinalAnimationID() const { return m_initialAnimationID; }

			float GetExitTime() const { return m_exitTime; }
			void SetExitTime(float value) { m_exitTime = value; }

			float GetTransitionTime() const { return m_transitionTime; }
			void SetTransitionTime(float value) { m_transitionTime = value; }

		private:

			static bool CheckCondition(const class Animator& animator, const AnimatorCondition& condition);

		private:

			DynamicArray<AnimatorCondition> m_conditions;

			float m_transitionTime;		

			float m_exitTime;
			ui32 m_initialAnimationID;
			ui32 m_finalAnimationID;

			friend class AnimatorSerializer;
		};
	}
}
