#pragma once

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

	protected:
		Timer m_gameStateTimer;
	};
}

