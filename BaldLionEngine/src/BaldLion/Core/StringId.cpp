#include "StringId.h"
#include <unordered_map>
#include "blpch.h"
#include <yaml-cpp/yaml.h>

#define YAML_KEY_STRINGID_MAP		"StringIdMap"
#define YAML_KEY_STRINGID_MAP_VALUE	"StringIdMapValue"

#define STRINGID_METADATA_PATH "assets/gameAssets/stringIds.meta"

namespace BaldLion {

	static std::unordered_map<StringId, const char*> s_stringIdTable;
	static std::mutex s_stringIdTableMutex;

	StringId StringToStringId(const char* str)
	{
		StringId sid = (ui32)std::hash<const char*>()(str);

		s_stringIdTableMutex.lock();
		std::unordered_map<StringId, const char*>::const_iterator it = s_stringIdTable.find(sid);

		if (it == s_stringIdTable.end())
		{
			s_stringIdTable[sid] = _strdup(str);
		}
		s_stringIdTableMutex.unlock();

		return sid;
	}

	StringId StringToStringId(const std::string& str)
	{
		return StringToStringId(str.c_str());
	}

	const char* StringIdToStrC(StringId sid)
	{
		std::unordered_map<StringId, const char*>::const_iterator it = s_stringIdTable.find(sid);
		BL_CORE_ASSERT(it != s_stringIdTable.end(), "String Id doesnt exist");
		const char* result = it->second;


		return result;
	}
	
	std::string StringIdToString(StringId sid) 
	{
		std::unordered_map<StringId, const char*>::const_iterator it = s_stringIdTable.find(sid);

		BL_CORE_ASSERT(it != s_stringIdTable.end(), "String Id doesnt exist");
		const char* result = it->second;

		return result;
	}

	void StringId::SerializeStringIdMap()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;

		out << YAML::Key << YAML_KEY_STRINGID_MAP << YAML::Value << YAML::BeginSeq;

		for(std::unordered_map<StringId, const char*>::const_iterator it = s_stringIdTable.begin(); it != s_stringIdTable.end(); ++it)		
		{
			out << YAML::BeginMap;
			out << YAML::Key << YAML_KEY_STRINGID_MAP_VALUE << YAML::Value << (std::string)(it->second);
			out << YAML::EndMap;
		}

		out << YAML::EndSeq;

		out << YAML::EndMap;

		std::ofstream fout(STRINGID_METADATA_PATH);
		fout << out.c_str();
		fout.close();
	}

	void StringId::DeserializeStringIdMap()
	{
		std::ifstream stream(STRINGID_METADATA_PATH);
		std::stringstream strStream;

		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());

		if (!data[YAML_KEY_STRINGID_MAP])
			return;

		auto stringIdMap = data[YAML_KEY_STRINGID_MAP];
		for (auto entry : stringIdMap)
		{
			BL_STRING_TO_STRINGID(entry[YAML_KEY_STRINGID_MAP_VALUE].as<std::string>().c_str());
		}

	}

}