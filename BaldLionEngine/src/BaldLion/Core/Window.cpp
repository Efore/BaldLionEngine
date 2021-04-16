#include "blpch.h"
#include "BaldLion/Core/Window.h"

#ifdef BL_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.h"
#endif

namespace BaldLion
{

	Window* Window::Create(const WindowProps& props)
	{
		#ifdef BL_PLATFORM_WINDOWS
			return MemoryManager::New<WindowsWindow>("Window",Memory::AllocationType::FreeList_Main, props);
		#else
			BL_CORE_ASSERT(false, "Unknown Platform!");
			return nullptr;
		#endif
	}

	void Window::Destroy(Window *window)
	{
		MemoryManager::Delete(window);
	}

}