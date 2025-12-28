#pragma once
#include "BaldLion/Core/Containers/HashMap.h"
#include "BaldLion/AI/HTN/HTNManager.h"
#include <yaml-cpp/yaml.h>

namespace BaldLion::AI::HTN
{	
	class HTNSerializer
	{

	private:
		static void SerializeHTNData();
		static bool DeserializeHTNData();

		static void SerializeHTNTasks(YAML::Emitter& out);		
		static void DeserializeHTNTask(const YAML::detail::iterator_value& yamlTask);

		static void SerializeHTNTask(const HTNTask& htnTask, YAML::Emitter& out);

		static void SerializeHTNDomains(YAML::Emitter& out);
		static void DeserializeHTNDomain(const YAML::detail::iterator_value& yamlDomain);

		static void SerializeWorldStateBlackboards(YAML::Emitter& out);
		static void DeserializeWorldStateBlackboard(const YAML::detail::iterator_value& yamlTask);

		friend class HTNManager;
	};
}