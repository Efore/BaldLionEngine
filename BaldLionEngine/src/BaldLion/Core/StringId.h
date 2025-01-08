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
	extern const char* StringIdToStrC(StringId sid);
	extern std::string StringIdToString(StringId sid);

#define BL_STRING_TO_STRINGID(x) ::BaldLion::StringToStringId(x)
#define BL_STRINGID_TO_STR_C(x) ::BaldLion::StringIdToStrC(x)
#define BL_STRINGID_TO_STRING(x) ::BaldLion::StringIdToString(x)
#define BL_STRINGID_NONE BL_STRING_TO_STRINGID("None");
}