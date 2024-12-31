#pragma once
#include "HTNTypes.h"

namespace BaldLion::AI::HTN
{
	class HTNOperator;
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
		enum TaskType
		{
			PrimitiveTask,
			CompoundTask
		};

		StringId taskID;
		TaskType type = TaskType::CompoundTask;

		DynamicArray<HTNMethod> methods;
		//OperatorFunc
		DynamicArray<HTNWorldStateEffect> effects;
		void ApplyEffects(HTNWorldStateBlackboard& worldStateBlackboard) const;
	};
}

