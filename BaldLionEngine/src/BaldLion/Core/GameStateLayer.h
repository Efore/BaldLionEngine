#pragma once

#include "BaldLion/Events/Event.h"
#include "BaldLion/Core/Time.h"

namespace BaldLion
{
	class  GameStateLayer
	{
	public:
		GameStateLayer();
		virtual ~GameStateLayer();

		virtual void OnActivate() {}
		virtual void OnDeactivate() {}
		virtual void OnUpdate() {}
		virtual void OnPause();
		virtual void OnResume();
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& e) {}

	protected:
		Timer m_gameStateTimer;
	};
}

