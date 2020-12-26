#pragma once

#include "BaldLion/Core/Window.h"
#include "BaldLion/Rendering/GraphicsContext.h"
#include <GLFW/glfw3.h>

namespace BaldLion
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline uint32_t GetWidth() const override { return m_data.Width; }
		inline uint32_t GetHeight() const override { return m_data.Height; }

		//Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		inline virtual void* GetNativeWindow() const { return m_window; }

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow * m_window;
		Rendering::GraphicsContext* m_context;

		struct WindowData
		{
			StringId Title;
			uint32_t Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_data;
	};
}