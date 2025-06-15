#include "StringId.h"
#include <unordered_map>
#include "blpch.h"

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
		s_stringIdTableMutex.lock();

		std::unordered_map<StringId, const char*>::const_iterator it = s_stringIdTable.find(sid);
		BL_CORE_ASSERT(it != s_stringIdTable.end(), "String Id doesnt exist");
		const char* result = it->second;

		s_stringIdTableMutex.unlock();

		return result;
	}
	
	std::string StringIdToString(StringId sid) {

		s_stringIdTableMutex.lock();

		std::unordered_map<StringId, const char*>::const_iterator it = s_stringIdTable.find(sid);

		BL_CORE_ASSERT(it != s_stringIdTable.end(), "String Id doesnt exist");
		const char* result = it->second;

		s_stringIdTableMutex.unlock();

		return result;
	}
}