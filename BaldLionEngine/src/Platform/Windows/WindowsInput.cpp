#include "blpch.h"

#include "BaldLion/Core/Input.h"
#include "BaldLion/Core/Application.h"
#include <GLFW/glfw3.h>

namespace BaldLion
{

	bool Input::IsKeyPressed(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::GetInstance().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonPress(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::GetInstance().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	std::pair<float, float> Input::GetMousePosition()
	{
		auto window = static_cast<GLFWwindow*>(Application::GetInstance().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float Input::GetMouseX()
	{
		auto mousePosition = GetMousePosition();
		return std::get<0>(mousePosition);
	}

	float Input::GetMouseY()
	{
		auto mousePosition = GetMousePosition();
		return std::get<1>(mousePosition);
	}
}