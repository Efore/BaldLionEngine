#pragma once
#include "HTNAgent.h"

namespace BaldLion::AI::HTN
{
	class HTNOperator
	{
		enum class OperatorType
		{
			Count
		};

		typedef void (*htnOperatorFunc)(HTNAgent& htnAgent);		

	public:
		htnOperatorFunc operatorFunc;
		StringId operatorID;
		OperatorType operatorType;
	};
}

