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
		enum TaskType : ui8
		{
			PrimitiveTask,
			CompoundTask
		};

		DynamicArray<HTNMethod> methods;		
		DynamicArray<HTNWorldStateEffect> effects;

		StringId taskID;
		TaskType taskType = TaskType::PrimitiveTask;

		HTNOperatorType taskOperatorType = HTNOperatorType::Count;

		void ApplyEffects(HTNWorldStateBlackboard& worldStateBlackboard) const;
	};
}

