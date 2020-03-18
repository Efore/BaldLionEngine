#include "blpch.h"
#include "BaldLion/Core/Window.h"

#ifdef BL_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.h"
#endif

namespace BaldLion
{

	Scope<Window> Window::Create(const WindowProps& props)
	{
		#ifdef BL_PLATFORM_WINDOWS
			return CreateScope<WindowsWindow>(props);
		#else
			BL_CORE_ASSERT(false, "Unknown Platform!");
			return nullptr;
		#endif
	}

}