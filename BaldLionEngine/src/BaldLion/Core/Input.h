#pragma once

#include "BaldLion/Core/Core.h"


namespace BaldLion
{
	class Input
	{
	public:
		static bool IsKeyPressed(int keycode);
		static bool IsMouseButtonPress(int button);
		static bool IsMouseButtonReleased(int button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();

	};
}