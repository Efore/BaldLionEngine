#pragma once
#include "BaldLion/Core/Containers/HashTable.h"
#include "BaldLion/Core/StringId.h"
#include "BaldLion/Core/Variant.h"

namespace BaldLion::AI::HTN
{
	using HTNWorldStateBlackboard = HashTable<StringId, Variant>;

	struct HTNWorldStateCondition
	{
		Variant value;
		StringId blackboardKey;
		VariantComparisonType comparisonType;
	};

	struct HTNWorldStateEffect
	{
		Variant blackboardValue;
		StringId blackboardKey;
	};

	struct HTNDomain
	{
		i32 mainTask = -1;
	};
	
	enum class HTNOperatorType : ui16
	{
		MoveTo,
		PlayAnimation,

		Count
	};

	struct HTNOperator
	{
		typedef void (*htnOperatorFunc)(class HTNAgent* htnAgent, const Variant* operatorData);

		htnOperatorFunc OperatorStartFunc;
		htnOperatorFunc OperatorInterruptFunc;
	};
}