#pragma once

#include "BaldLion/Layer.h"

#include "BaldLion/Events/KeyEvent.h"
#include "BaldLion/Events/MouseEvent.h"
#include "BaldLion/Events/ApplicationEvent.h"

namespace BaldLion
{
	class BL_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();

	private:
		float m_time = 0.0f;
		
	};
}
