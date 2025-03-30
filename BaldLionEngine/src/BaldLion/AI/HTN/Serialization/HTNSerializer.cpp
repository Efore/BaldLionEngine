#include "blpch.h"
#include "HTNSerializer.h"

namespace BaldLion::AI::HTN
{
	void HTNSerializer::SerializeDefinedHTNTasks(const DynamicArray<HTNTask>& tasks)
	{
		YAML::Emitter out;

		out << YAML::BeginMap;

		//out << YAML::Key << YAML_KEY_TASKS << YAML::Value << YAML::BeginSeq;

		//BL_DYNAMICARRAY_FOR(tasks, it)
		//{
		//	out << YAML::BeginMap;
		//	out << YAML::Key << YAML_KEY_PARAMETER_ID << YAML::Value << BL_STRINGID_TO_STR_C(it.GetKey());
		//	SerializeParameter(out, it.GetValue());
		//	out << YAML::EndMap;
		//}

		//out << YAML::EndSeq;


		//const std::string parentFolderPath = StringUtils::GetPathWithoutFile(filepath);

		//if (CreateDirectoryA(parentFolderPath.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
		//{
		//	std::ofstream fout(filepath);
		//	fout << out.c_str();
		//	fout.close();
		//}
	}

	void HTNSerializer::DeserializeDefinedHTNTasks(DynamicArray<HTNTask>& tasks)
	{

	}
	void HTNSerializer::SerializeHTNTask(const HTNTask& tasks)
	{
		
	}

	void HTNSerializer::DeserializeHTNTask(HTNTask& tasks)
	{

	}

	void HTNSerializer::SerializeHTNDomains(const HashMap<StringId, HTNDomain>& domains)
	{

	}

	bool HTNSerializer::DeserializeHTNDomains(HashMap<StringId, HTNDomain>& domains)
	{
		return false;
	}


}