#pragma once

#ifdef BL_PLATFORM_WINDOWS
	#ifdef BL_BUILD_DLL
		#define BL_API __declspec(dllexport)
	#else
		#define BL_API __declspec(dllimport)
	#endif
#else
	#error BaldLion only support Wiwndos!
#endif

#ifdef BL_ENABLE_ASSERTS
	#define BL_ASSERT(x, ...) { if(!(x)) { BL_LOG_ERROR ("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define BL_CORE_ASSERT(x, ...) { if(!(x)) { BL_LOG_CORE_ERROR ("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define BL_ASSERT(x, ...)
	#define BL_CORE_ASSERT(x,...)
#endif

#define BIT(x) (1 << x)