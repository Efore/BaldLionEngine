#pragma once

#include "BaldLion/Core/GameStateLayer.h"

#include "BaldLion/Events/KeyEvent.h"
#include "BaldLion/Events/MouseEvent.h"
#include "BaldLion/Events/ApplicationEvent.h"

namespace BaldLion
{
	class  ImGuiLayer : public GameStateLayer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnActivate() override;
		virtual void OnDeactivate() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();

	private:
		float m_time = 0.0f;
		
	};
}
