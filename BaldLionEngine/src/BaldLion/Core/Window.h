#pragma once

#include "blpch.h"

#include "BaldLion/Core/Core.h"
#include "BaldLion/Events/Event.h"

namespace BaldLion
{
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "BaldLion Engine",
			unsigned int width = 1366,
			unsigned int height = 768)
			: Title(title), Width(width), Height(height)
		{

		}
	};

	//Interface representing a desktop system based Window
	class BL_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		//Windows attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Scope<Window> Create(const WindowProps& props = WindowProps());		
	};
}