#pragma once
#include <tuple>
#include "Log.h"

#define BL_ENABLE_ASSERTS

#ifdef BL_ENABLE_ASSERTS
	#define BL_ASSERT(x) { if(!(x)) { __debugbreak(); } }
	#define BL_ASSERT_LOG(x, ...) { if(!(x)) { BL_LOG_ERROR ("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define BL_CORE_ASSERT(x, ...) { if(!(x)) { BL_LOG_CORE_ERROR ("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define BL_ASSERT(x) 
	#define BL_ASSERT(x, ...)
	#define BL_CORE_ASSERT(x,...)
#endif

#define BIT(x) (1 << x)

#define BL_BIND_FUNCTION(fn) std::bind(&fn, this, std::placeholders::_1)
#define BL_NUMARGS(...) std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value

using ui64 = uint64_t;
using i64 = int64_t;
using ui32 = uint32_t;
using i32 = int32_t;
using ui16 = uint16_t;
using i16 = int16_t;
using ui8 = uint8_t;
using i8 = int8_t;
using hashType = size_t; 

