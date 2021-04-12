#pragma once

#include "optick.h"

#define BL_PROFILE_FRAME() OPTICK_FRAME("Main thread")
#define BL_PROFILE_THREAD(Name) OPTICK_THREAD(Name)
#define BL_PROFILE_FUNCTION() OPTICK_EVENT()
#define BL_PROFILE_SCOPE(Name, Category) OPTICK_CATEGORY(Name, Category)
