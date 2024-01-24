#pragma once

#include "optick.h"

#define BL_PROFILE_ACTIVE 0

#if BL_PROFILE_ACTIVE
#define BL_PROFILE_FRAME() OPTICK_FRAME("Main thread")
#define BL_PROFILE_THREAD(Name) OPTICK_THREAD(Name)
#define BL_PROFILE_FUNCTION() OPTICK_EVENT()
#define BL_PROFILE_SCOPE(Name, Category) OPTICK_CATEGORY(Name, Category)
#else
#define BL_PROFILE_FRAME() 
#define BL_PROFILE_THREAD(Name)
#define BL_PROFILE_FUNCTION() 
#define BL_PROFILE_SCOPE(Name, Category) 
#endif

#define BL_DEEP_PROFILE 1

#if BL_DEEP_PROFILE
	#define BL_DEEP_PROFILE_FUNCTION() BL_PROFILE_FUNCTION()
	#define BL_DEEP_PROFILE_SCOPE(Name, Category) BL_PROFILE_SCOPE(Name, Category)
#else
	#define BL_DEEP_PROFILE_FUNCTION() 
	#define BL_DEEP_PROFILE_SCOPE(Name, Category) 
#endif
