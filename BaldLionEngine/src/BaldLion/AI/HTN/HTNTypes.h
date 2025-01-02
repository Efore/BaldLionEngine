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

	struct HTNWorldStateProperty
	{
		StringId blackboardKey;
		Variant blackboardValue;
	};

	struct HTNDomain
	{
		StringId domainID;
		ui32 mainTask;
	};
}