#pragma once
#include "Log.h"

#define BL_ENABLE_ASSERTS

#ifdef BL_ENABLE_ASSERTS
	#define BL_ASSERT(x, ...) { if(!(x)) { BL_LOG_ERROR ("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define BL_CORE_ASSERT(x, ...) { if(!(x)) { BL_LOG_CORE_ERROR ("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define BL_ASSERT(x, ...)
	#define BL_CORE_ASSERT(x,...)
#endif

#define BIT(x) (1 << x)

#define BL_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

