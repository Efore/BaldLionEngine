#pragma once

#include "BaldLion/Core/Layer.h"

#include "BaldLion/Events/KeyEvent.h"
#include "BaldLion/Events/MouseEvent.h"
#include "BaldLion/Events/ApplicationEvent.h"

namespace BaldLion
{
	class  ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender(TimeStep timeStep) override;

		void Begin();
		void End();

	private:
		float m_time = 0.0f;
		
	};
}
