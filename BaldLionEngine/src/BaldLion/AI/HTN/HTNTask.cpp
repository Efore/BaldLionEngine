#include "blpch.h"
#include "HTNTask.h"

namespace BaldLion::AI::HTN
{
	bool HTNMethod::CheckConditions(const HTNWorldStateBlackboard& worldStateBlackboard) const
	{
		BL_DYNAMICARRAY_FOREACH(conditions)
		{
			const HTNWorldStateCondition& condition = conditions[i];
			Variant blackboardEntry;
			if (worldStateBlackboard.TryGet(condition.blackboardKey, blackboardEntry))
			{
				switch (condition.comparisonType)
				{
				case VariantComparisonType::Lower:
					if (blackboardEntry >= condition.value)
					{
						return false;
					}
					break;
				case VariantComparisonType::LowerOrEqual:
					if (blackboardEntry > condition.value)
					{
						return false;
					}
					break;
				case VariantComparisonType::Equal:
					if (blackboardEntry != condition.value)
					{
						return false;
					}
					break;
				case VariantComparisonType::Unequal:
					if (blackboardEntry == condition.value)
					{
						return false;
					}
					break;
				case VariantComparisonType::HigherOrEqual:
					if (blackboardEntry < condition.value)
					{
						return false;
					}
					break;
				case VariantComparisonType::Higher:
					if (blackboardEntry <= condition.value)
					{
						return false;
					}
					break;
				}
			}
		}
		return true;
	}

	void HTNTask::ApplyEffects(HTNWorldStateBlackboard& worldStateBlackboard) const
	{
		BL_DYNAMICARRAY_FOREACH(effects)
		{
			const HTNWorldStateEffect& effect = effects[i];
			Variant* blackboardEntry;
			if (worldStateBlackboard.TryGet(effect.blackboardKey, blackboardEntry))
			{
				(*blackboardEntry) = effect.blackboardValue;
			}
		}
	}
}