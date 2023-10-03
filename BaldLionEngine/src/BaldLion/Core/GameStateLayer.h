#pragma once

#include "BaldLion/Events/Event.h"

namespace BaldLion
{
	class  GameStateLayer
	{
	public:
		GameStateLayer();
		virtual ~GameStateLayer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& e) {}
	};
}

