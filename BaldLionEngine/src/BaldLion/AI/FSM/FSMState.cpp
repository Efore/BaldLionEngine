#include "blpch.h"
#include "FSMState.h"

namespace BaldLion
{
	namespace AI
	{
		FSMState::FSMState(const char* stateName, const FSMStateFunction& onEnterStateFunction, const FSMStateFunction& onUpdateStateFunction, const FSMStateFunction& onExitStateFunction) :
			m_stateID(BL_STRING_TO_STRINGID(stateName)),
			m_onEnterStateFunction(onEnterStateFunction), 
			m_onUpdateStateFunction(onUpdateStateFunction),
			m_onExitStateFunction(onExitStateFunction)
		{
		}

		FSMState::~FSMState()
		{
		}


		void FSMState::OnEnterState()
		{
			if (m_onEnterStateFunction != nullptr)
				m_onEnterStateFunction();
		}

		void FSMState::OnUpdateState()
		{
			if (m_onUpdateStateFunction != nullptr)
				m_onUpdateStateFunction();
		}

		void FSMState::OnExitState()
		{
			if (m_onExitStateFunction != nullptr)
				m_onExitStateFunction();
		}

	}
}

