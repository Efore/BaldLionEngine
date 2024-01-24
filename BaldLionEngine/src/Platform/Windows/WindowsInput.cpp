#include "blpch.h"

#include "BaldLion/Core/Input.h"
#include "BaldLion/Core/Application.h"
#include <GLFW/glfw3.h>

namespace BaldLion
{
	bool Input::PlatformInput::IsKeyPressed(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::GetInstance().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS;
	}

	bool Input::PlatformInput::IsMouseButtonPress(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::GetInstance().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	bool BaldLion::Input::PlatformInput::IsMouseButtonReleased(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::GetInstance().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_RELEASE;
	}

	std::pair<float, float> Input::PlatformInput::GetMousePosition()
	{
		auto window = static_cast<GLFWwindow*>(Application::GetInstance().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float Input::PlatformInput::GetMouseX()
	{
		auto mousePosition = GetMousePosition();
		return std::get<0>(mousePosition);
	}

	float Input::PlatformInput::GetMouseY()
	{
		auto mousePosition = GetMousePosition();
		return std::get<1>(mousePosition);
	}

	float Input::PlatformInput::GetGamepadAxisValue(int gamepadIndex, int button)
	{
		float result = 0.0f;

		GLFWgamepadstate state;

		if (glfwGetGamepadState(gamepadIndex, &state))
		{
			result = state.axes[button];
		}

		return result;
	}

	bool Input::PlatformInput::GetGamepadButtonValue(int gamepadIndex, int button)
	{
		bool result = false;

		GLFWgamepadstate state;

		if (glfwGetGamepadState(gamepadIndex, &state))
		{
			result = state.buttons[button];
		}

		return result;
	}
}