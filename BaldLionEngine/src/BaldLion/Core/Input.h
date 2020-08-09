#pragma once

#include "BaldLion/Core/Core.h"


namespace BaldLion
{
	class BL_API Input
	{
	public:
		static bool IsKeyPressed(int keycode);
		static bool IsMouseButtonPress(int button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();

	};
}