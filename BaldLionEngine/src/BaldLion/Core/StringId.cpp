#include "StringId.h"
#include <unordered_map>
#include "blpch.h"

namespace BaldLion {

	static std::unordered_map<StringId, const char*> s_stringIdTable;

	StringId StringToStringId(const char* str)
	{
		StringId sid = std::hash<const char*>()(str);
		std::unordered_map<StringId, const char*>::const_iterator it = s_stringIdTable.find(sid);

		if (it == s_stringIdTable.end())
		{
			s_stringIdTable[sid] = _strdup(str);
		}

		return sid;
	}

	StringId StringToStringId(const std::string& str)
	{
		return StringToStringId(str.c_str());
	}

	const char* StringIdToString(StringId sid) 
	{
		std::unordered_map<StringId, const char*>::const_iterator it = s_stringIdTable.find(sid);

		BL_CORE_ASSERT(it != s_stringIdTable.end(), "String Id doesnt exist");

		return it->second;
	}
}