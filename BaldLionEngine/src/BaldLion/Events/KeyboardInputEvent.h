#pragma once

#include "Event.h"

namespace BaldLion
{
	class  KeyboardInputEvent : public Event
	{
	public:
		inline int GetKeyCode() const { return m_keyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)		

	protected:
		KeyboardInputEvent(int keycode) : m_keyCode(keycode){}
		int m_keyCode;
	};

	class  KeyPressedEvent : public KeyboardInputEvent
	{
	public:
		KeyPressedEvent(int keycode, int repeatCount) : KeyboardInputEvent(keycode), m_repeatCount(repeatCount){}

		inline int GetRepeatCount() const { return m_repeatCount; }

		std::string ToString() const override 
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_keyCode << " (" << m_repeatCount << "repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)

	private:
		int m_repeatCount;
	};

	class  KeyReleasedEvent : public KeyboardInputEvent
	{
	public:
		KeyReleasedEvent(int keycode) : KeyboardInputEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_keyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class  KeyTypedEvent : public KeyboardInputEvent
	{
	public:
		KeyTypedEvent(int keycode) : KeyboardInputEvent(keycode) {}
		
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_keyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}