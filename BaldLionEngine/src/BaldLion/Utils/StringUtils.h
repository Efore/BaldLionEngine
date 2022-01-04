#pragma once
#include <string>

namespace BaldLion
{
	namespace StringUtils 
	{
		static std::string GetFileNameFromPath(const std::string &path, bool includeExt = false)
		{
			// Extracting name from last path
			auto lastSlash = path.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			auto lastDot = path.rfind('.');
			auto count = lastDot == std::string::npos || includeExt ? path.size() - lastSlash : lastDot - lastSlash;
			return path.substr(lastSlash, count);			
		}

		static std::string GetPathWithoutExtension(const std::string &path)
		{
			auto lastDot = path.rfind('.');
			return path.substr(0, lastDot);
		}
	}
}