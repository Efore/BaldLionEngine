#include "blpch.h"
#include "WindowsInput.h"

#include "BaldLion/Core/Application.h"
#include <GLFW/glfw3.h>

namespace BaldLion
{
	Input* Input::m_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::GetInstance().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS;
	}

	bool WindowsInput::IsMousePressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::GetInstance().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	std::pair<float, float> WindowsInput::GetMousePositionImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::GetInstance().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float WindowsInput::GetMouseXImpl()
	{
		auto mousePosition = GetMousePosition();
		return std::get<0>(mousePosition);
	}

	float WindowsInput::GetMouseYImpl()
	{
		auto mousePosition = GetMousePosition();
		return std::get<1>(mousePosition);
	}
}