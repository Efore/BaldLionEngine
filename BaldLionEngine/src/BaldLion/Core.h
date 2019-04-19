#pragma once

#ifdef BL_PLATFORM_WINDOWS
	#if BL_DYNAMIC_LINK
		#ifdef BL_BUILD_DLL
			#define BL_API __declspec(dllexport)
		#else
			#define BL_API __declspec(dllimport)
		#endif
	#else
		#define BL_API
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

#define BL_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)