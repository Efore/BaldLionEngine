#pragma once
#include "BaldLion/Core/Containers/DynamicArray.h"
#include "FSMState.h"
#include "FSMTransition.h"

namespace BaldLion
{
	namespace AI 
	{
		class FSMController 
		{
		public:
			FSMController();
			~FSMController();

			void AddState(FSMState&& state);
			void AddTransition(ui32 initialState, ui32 finalState, const std::function<bool()>& condition);

			void UpdateFSM();

		private:

			void CheckConditions();

		private:

			HashTable<ui32,FSMState> m_states;
			HashTable<ui32, FSMTransition> m_stateTransitions;			
			FSMState* m_currentState;
		};
	}
}
