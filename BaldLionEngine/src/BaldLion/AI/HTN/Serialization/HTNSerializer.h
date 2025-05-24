#pragma once
#include "BaldLion/Core/Containers/HashMap.h"
#include "BaldLion/AI/HTN/HTNManager.h"
#include <yaml-cpp/yaml.h>

namespace BaldLion::AI::HTN
{
	#define HTN_DATA_PATH "assets/gameAssets/htnData.meta"

	class HTNSerializer
	{

	private:
		static void SerializeHTNData();
		static bool DeserializeHTNData();

		static void SerializeDefinedHTNTasks(YAML::Emitter& out);		

		static void SerializeHTNTask(const HTNTask& htnTask, YAML::Emitter& out);
		static void DeserializeHTNTask(const YAML::detail::iterator_value& yamlTask);

		static void SerializeHTNDomains(YAML::Emitter& out);
		static void DeserializeHTNDomain(const YAML::detail::iterator_value& yamlDomain);

		static void SerializeVariant(YAML::Emitter& out, const std::string& key, const Variant& variant);
		static void DeserializeVariant(const YAML::Node& node, const std::string& key, Variant& result);

		friend class HTNManager;
	};
}