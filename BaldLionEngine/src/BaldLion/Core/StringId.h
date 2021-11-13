#pragma once
#include "Core.h"

typedef ui32 StringId; 

namespace std
{
	template <>
	struct hash<const char *>
	{
		size_t operator()(const char *s) const
		{
			size_t h = 5381;
			int c;
			while ((c = *s++))
				h = ((h << 5) + h) + c;
			return h;
		}
	};

	template <>
	struct hash<char *>
	{
		size_t operator()(char *s) const
		{
			size_t h = 5381;
			int c;
			while ((c = *s++))
				h = ((h << 5) + h) + c;
			return h;
		}
	};

}

namespace BaldLion {
	extern StringId StringToStringId(const char* str);
	extern StringId StringToStringId(const std::string& str);
	extern const char* StringIdToString(StringId sid);
}

#define STRING_TO_STRINGID(x) ::BaldLion::StringToStringId(x)
#define STRINGID_TO_STRING(x) ::BaldLion::StringIdToString(x)