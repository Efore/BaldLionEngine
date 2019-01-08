#pragma once

#include "BaldLion/Layer.h"

namespace BaldLion
{
	class BL_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);

		void MouseMovedCallback(float posX, float posY);
		void MouseButtonCallback(int button, int action);
		void ScrollCallback(double xoffset, double yoffset);
		void KeyCallback(int key, int action);
		void CharCallback(unsigned int c);
		void WindowsCloseCallback();

	private:
		float m_time = 0.0f;
		
	};
}
