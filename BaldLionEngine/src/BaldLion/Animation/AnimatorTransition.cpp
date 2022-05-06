#include "blpch.h"
#include "AnimatorTransition.h"
#include "Animator.h"

namespace BaldLion
{
	namespace Animation 
	{
		AnimatorTransition::AnimatorTransition():
			m_initialAnimationID(0),
			m_finalAnimationID(0),
			m_exitTime(0.0f),
			m_transitionTime(0.0f)
		{

		}

		AnimatorTransition::AnimatorTransition(ui32 initialAnimationID, ui32 finalAnimationID,float exitTime, float transitionTime) :
			m_initialAnimationID(initialAnimationID),
			m_finalAnimationID(finalAnimationID),
			m_exitTime(exitTime),
			m_transitionTime(transitionTime)
		{
			m_conditions = DynamicArray<AnimatorCondition>(Memory::MemoryManager::GetAllocatorType(this), 10);
		}

		AnimatorTransition::AnimatorTransition(AnimatorTransition&& other)
		{
			m_initialAnimationID = other.m_initialAnimationID;
			m_finalAnimationID = other.m_finalAnimationID;
			m_transitionTime = other.m_transitionTime;
			m_conditions = std::move(other.m_conditions);
		}

		AnimatorTransition::~AnimatorTransition()
		{
			m_conditions.Delete();
		}

		AnimatorTransition& AnimatorTransition::operator=(AnimatorTransition&& other)
		{
			m_initialAnimationID = other.m_initialAnimationID;
			m_finalAnimationID = other.m_finalAnimationID;
			m_transitionTime = other.m_transitionTime;
			m_conditions = std::move(other.m_conditions);

			return *this;
		}

		AnimatorTransition& AnimatorTransition::operator=(const AnimatorTransition& other)
		{
			m_initialAnimationID = other.m_initialAnimationID;
			m_finalAnimationID = other.m_finalAnimationID;
			m_transitionTime = other.m_transitionTime;
			m_conditions = other.m_conditions;

			return *this;
		}

		bool AnimatorTransition::operator==(const AnimatorTransition& other)
		{
			return m_initialAnimationID == other.m_initialAnimationID && m_finalAnimationID == other.m_finalAnimationID;
		}


		bool AnimatorTransition::operator!=(const AnimatorTransition& other)
		{
			return m_initialAnimationID != other.m_initialAnimationID || m_finalAnimationID != other.m_finalAnimationID;
		}

		void AnimatorTransition::AddCondition(const AnimatorCondition& condition)
		{
			m_conditions.PushBack(condition);
		}

		bool AnimatorTransition::CheckConditions(const Animator& animator, float animatorTime) const
		{
			if (m_exitTime > 0.0f && animatorTime < m_exitTime)
				return false;

			BL_DYNAMICARRAY_FOR(i, m_conditions, 0)
			{
				if (!AnimatorTransition::CheckCondition(animator, m_conditions[i]))
					return false;
			}

			return true;
		}

		void AnimatorTransition::RemoveCondition(ui32 conditionIndex)
		{
			if (conditionIndex < m_conditions.Size())
			{
				m_conditions.RemoveAtFast(conditionIndex);
			}
		}

		bool AnimatorTransition::CheckCondition(const class Animator& animator, const AnimatorCondition& condition)
		{
			AnimatorParameter parameterA = animator.GetParameter(condition.ParameterAName);
			AnimatorParameter parameterB = condition.ParameterB;

			switch (parameterA.Type)
			{

			case AnimatorParameter::ValueType::Bool:
			{
				bool valueBoolA = parameterA.Value.boolean;
				bool valueBoolB = parameterB.Value.boolean;

				switch (condition.Comparison)
				{

				case AnimatorCondition::ComparisonType::Equal:
					return valueBoolA == valueBoolB;
					break;

				case AnimatorCondition::ComparisonType::Unequal:
					return valueBoolA != valueBoolB;
					break;
				}

			}
			break;

			case AnimatorParameter::ValueType::Int:
			{
				int valueIntA = parameterA.Value.integer;
				int valueIntB = parameterB.Value.integer;

				switch (condition.Comparison)
				{
				case AnimatorCondition::ComparisonType::Lower:
					return valueIntA < valueIntB;
					break;
				case AnimatorCondition::ComparisonType::LowerOrEqual:
					return valueIntA <= valueIntB;
					break;
				case AnimatorCondition::ComparisonType::Equal:
					return valueIntA == valueIntB;
					break;
				case AnimatorCondition::ComparisonType::Unequal:
					return valueIntA != valueIntB;
					break;
				case AnimatorCondition::ComparisonType::HigherOrEqual:
					return valueIntA >= valueIntB;
					break;
				case AnimatorCondition::ComparisonType::Higher:
					return valueIntA > valueIntB;
					break;
				}
			}
			break;

			case AnimatorParameter::ValueType::Float:
			{
				float valueFloatA = parameterA.Value.floating;
				float valueFloatB = parameterB.Value.floating;

				switch (condition.Comparison)
				{
				case AnimatorCondition::ComparisonType::Lower:
					return valueFloatA < valueFloatB;
					break;
				case AnimatorCondition::ComparisonType::LowerOrEqual:
					return valueFloatA <= valueFloatB;
					break;
				case AnimatorCondition::ComparisonType::Equal:
					return valueFloatA == valueFloatB;
					break;
				case AnimatorCondition::ComparisonType::Unequal:
					return valueFloatA != valueFloatB;
					break;
				case AnimatorCondition::ComparisonType::HigherOrEqual:
					return valueFloatA >= valueFloatB;
					break;
				case AnimatorCondition::ComparisonType::Higher:
					return valueFloatA > valueFloatB;
					break;
				}
			}
			break;

			}

			return false;
		}
	}
}
