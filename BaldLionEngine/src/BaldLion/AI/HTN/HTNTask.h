#pragma once
#include "HTNTypes.h"

namespace BaldLion::AI::HTN
{
	class HTNTask;

	class HTNMethod
	{
	public:
		bool CheckConditions(const HTNWorldStateBlackboard& worldStateBlackboard) const;

		DynamicArray<HTNWorldStateCondition> conditions;
		DynamicArray<ui32> subtasksIndices;		
	};

	class HTNTask
	{
	public:
		enum TaskType : ui16
		{
			PrimitiveTask,
			CompoundTask
		};

		DynamicArray<HTNMethod> methods;		
		DynamicArray<HTNWorldStateEffect> effects;

		StringId taskID;
		TaskType taskType = TaskType::PrimitiveTask;

		HTNOperatorType taskOperatorType = HTNOperatorType::MoveTo;
		Variant taskOperatorData[4];
		

		void ApplyEffects(HTNWorldStateBlackboard& worldStateBlackboard) const;
	};
}

