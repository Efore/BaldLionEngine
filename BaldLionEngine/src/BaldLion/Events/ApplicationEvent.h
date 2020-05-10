#pragma once

#include "Event.h"

namespace BaldLion
{
	class BL_API WindowResizeEvent : public Event
	{
	public: 
		WindowResizeEvent(unsigned int width, unsigned int height) : m_width(width), m_height(height){}

		inline unsigned int GetWidth() const { return m_width; }
		inline unsigned int GetHeight() const { return m_height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_width << ", " << m_height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		unsigned int m_width, m_height;
	};

	class BL_API WindowMinimizeEvent : public Event
	{
	public:
		WindowMinimizeEvent(bool minimized) : m_minimized(minimized) {}

		inline bool GetMinimized() const { return m_minimized; }

		std::string ToString() const override
		{
			std::stringstream ss;
			if (m_minimized)
			{
				ss << "WindowMinimizeEvent: true";
			}
			else
			{
				ss << "WindowMinimizeEvent: false";
			}
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowMinimize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		bool m_minimized;

	};

	class BL_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent(){}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
	
	class BL_API AppTickEvent : public Event
	{
	public:
		AppTickEvent(){}

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class BL_API AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent(){}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class BL_API AppRenderEvent : public Event
	{
	public:
		AppRenderEvent(){}

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};


}