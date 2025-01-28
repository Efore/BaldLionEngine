#pragma once
#include "BaldLion/Core/Core.h"

namespace BaldLion
{
	namespace Animation
	{
		struct AnimatorParameter {

			union {
				ui32 integer;
				float floating;
				bool boolean;
			} Value;

			enum class ValueType : ui8
			{
				Int = 0,
				Float = 1,
				Bool = 2
			} Type;
		};

		struct AnimatorCondition {

			enum class ComparisonType : ui8 {
				Lower = 0,
				LowerOrEqual = 1,
				Equal = 2,
				Unequal = 3,
				HigherOrEqual = 4,
				Higher = 5
			} Comparison;

			StringId ParameterAName;
			AnimatorParameter ParameterB;
		};
	}
}


