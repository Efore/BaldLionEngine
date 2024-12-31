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
		VariantComparisonType comparisonType;
		StringId blackboardKey;
	};

	struct HTNWorldStateEffect
	{
		Variant newValue;
		StringId blackboardKey;
	};

	struct HTNDomain
	{
		StringId domainID;
		ui32 mainTask;;
	};
}