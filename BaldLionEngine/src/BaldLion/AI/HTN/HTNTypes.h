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
		StringId blackboardKey;
		Variant blackboardValue;
	};

	struct HTNDomain
	{
		i32 mainTask = -1;
	};
	
	enum class HTNOperatorType : ui32
	{
		MoveTo,
		PlayAnimation,

		Count
	};

	struct HTNOperator
	{
		typedef void (*htnOperatorFunc)(class HTNAgent* htnAgent);

		htnOperatorFunc OperatorStartFunc;
		htnOperatorFunc OperatorInterruptFunc;
	};
}