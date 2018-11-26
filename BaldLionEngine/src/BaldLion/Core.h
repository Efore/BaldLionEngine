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

#define BIT(x) (1 << x)