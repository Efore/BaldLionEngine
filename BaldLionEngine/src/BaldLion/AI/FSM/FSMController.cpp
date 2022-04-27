#include "blpch.h"
#include "FSMController.h"

namespace BaldLion 
{
	namespace AI
	{
		FSMController::FSMController()
		{
			m_states = HashTable<ui32, FSMState>(Memory::MemoryManager::GetAllocatorType(this), 10);
			m_stateTransitions = HashTable<ui32, FSMTransition>(Memory::MemoryManager::GetAllocatorType(this), 10);
		}

		FSMController::~FSMController()
		{
			m_states.Delete();
			m_stateTransitions.Delete();
		}

		void FSMController::AddState(FSMState&& state)
		{
			m_states.Emplace(state.GetStateID(),std::move(state));			
		}

		void FSMController::AddTransition(ui32 initialState, ui32 finalState, const std::function<bool()>& condition)
		{
			m_states.Emplace(initialState, initialState, finalState, condition);
		}

		void FSMController::UpdateFSM()
		{
			if (m_currentState != nullptr)
			{
				m_currentState->OnUpdateState();
				CheckConditions();
			}
		}

		void FSMController::CheckConditions()
		{
			FSMTransition* transition = nullptr;
			if (m_stateTransitions.TryGet(m_currentState->GetStateID(), *transition))
			{
				if (transition->CheckCondition())
				{
					m_currentState->OnExitState();
					m_currentState = &m_states.Get(transition->GetFinalState());
					m_currentState->OnEnterState();
				}
			}
		}
	}
}

