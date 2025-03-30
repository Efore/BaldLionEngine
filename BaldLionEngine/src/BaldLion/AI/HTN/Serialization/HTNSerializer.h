#pragma once
#include "BaldLion/Core/Containers/HashMap.h"
#include "BaldLion/AI/HTN/HTNTypes.h"
#include "BaldLion/AI/HTN/HTNTask.h"
#include <yaml-cpp/yaml.h>

namespace BaldLion::AI::HTN
{
	class HTNSerializer
	{

	private:
		static void SerializeDefinedHTNTasks(const DynamicArray<HTNTask>& tasks);
		static void DeserializeDefinedHTNTasks(DynamicArray<HTNTask>& tasks);

		static void SerializeHTNTask(const HTNTask& tasks);
		static void DeserializeHTNTask(HTNTask& tasks);

		static void SerializeHTNDomains(const HashMap<StringId, HTNDomain>& domains);
		static bool DeserializeHTNDomains(HashMap<StringId, HTNDomain>& domains);

		friend class HTNManager;
	};
}