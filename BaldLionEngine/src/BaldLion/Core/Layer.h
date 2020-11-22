#pragma once

#include "BaldLion/Events/Event.h"
#include "BaldLion/Core/TimeStep.h"

namespace BaldLion
{
	class  Layer
	{
	public:
		Layer();
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(TimeStep timeStep) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& e) {}
	};
}

