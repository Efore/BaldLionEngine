#pragma once

#include "blpch.h"

#include "BaldLion/Core/Core.h"
#include "BaldLion/Events/Event.h"

namespace BaldLion
{
	struct WindowProps
	{
		StringId Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "",
			uint32_t width = 1366,
			uint32_t height = 768)
			: Title(STRING_TO_ID(title)), Width(width), Height(height)
		{

		}
	};

	//Interface representing a desktop system based Window
	class  Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		//Windows attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());		
		static void Destroy(Window *window);
	};
}
