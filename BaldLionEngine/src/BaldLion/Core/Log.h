#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace BaldLion
{
	class  Log
	{
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:		
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

//Core log macros
#define BL_LOG_CORE_TRACE(...)		::BaldLion::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define BL_LOG_CORE_INFO(...)		::BaldLion::Log::GetCoreLogger()->info(__VA_ARGS__)
#define BL_LOG_CORE_WARN(...)		::BaldLion::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define BL_LOG_CORE_ERROR(...)		::BaldLion::Log::GetCoreLogger()->error(__VA_ARGS__)
#define BL_LOG_CORE_FATAL(...)		::BaldLion::Log::GetCoreLogger()->fatal(__VA_ARGS__)

//Client log macros
#define BL_LOG_TRACE(...)			::BaldLion::Log::GetClientLogger()->trace(__VA_ARGS__)
#define BL_LOG_INFO(...)			::BaldLion::Log::GetClientLogger()->info(__VA_ARGS__)
#define BL_LOG_WARN(...)			::BaldLion::Log::GetClientLogger()->warn(__VA_ARGS__)
#define BL_LOG_ERROR(...)			::BaldLion::Log::GetClientLogger()->error(__VA_ARGS__)
#define BL_LOG_FATAL(...)			::BaldLion::Log::GetClientLogger()->fatal(__VA_ARGS__)

