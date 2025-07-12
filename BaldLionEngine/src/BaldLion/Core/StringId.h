#pragma once
#include "Core.h"

namespace BaldLion
{
	struct StringId
	{
		StringId() = default;

		StringId(ui32 value) : value(value){}

		StringId& operator=(ui32 value)
		{
			value = value;
			return *this;
		}

		operator ui32() const
		{
			return value;
		}

	private:
		ui32 value;

	};
}

namespace std
{
	template <>
	struct hash<BaldLion::StringId>
	{
		size_t operator()(BaldLion::StringId stringID) const
		{
			return std::hash<i32>()((ui32)stringID);
		}
	};

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