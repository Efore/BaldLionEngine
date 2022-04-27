#include "blpch.h"
#include "FSMTransition.h"

namespace BaldLion
{
	namespace AI
	{
		FSMTransition::FSMTransition(ui32 initialState, ui32 finalState, const std::function<bool()>& condition) :
			m_initialState(initialState), m_finalState(finalState), m_condition(condition)
		{
		}

		FSMTransition::~FSMTransition()
		{
		}


		bool FSMTransition::CheckCondition()
		{
			return m_condition();
		}
	}
}
