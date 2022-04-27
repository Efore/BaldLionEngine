#pragma once
#include <functional>
#include "FSMState.h"

namespace BaldLion
{
	namespace AI 
	{
		class FSMTransition 
		{
		public:
			FSMTransition(ui32 initialState, ui32 finalState, const std::function<bool()>& condition);
			~FSMTransition();

			bool CheckCondition();
			ui32 GetFinalState() const { return m_finalState; }

		private:
			ui32 m_initialState;
			ui32 m_finalState;
			std::function<bool()> m_condition;
		};
	}
}
