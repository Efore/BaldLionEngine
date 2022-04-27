#pragma once
#include <functional>
#include "BaldLion/Core/Core.h"

namespace BaldLion
{
	namespace AI 
	{
		typedef std::function<void()> FSMStateFunction;

		class FSMState 
		{
		public:
			FSMState(const char* stateName, const FSMStateFunction& onEnterStateFunction, const FSMStateFunction& onUpdateStateFunction, const FSMStateFunction& onExitStateFunction);
			~FSMState();

			void OnEnterState();
			void OnUpdateState();
			void OnExitState();

			ui32 GetStateID() const { return m_stateID; }

		protected:

			FSMStateFunction m_onEnterStateFunction;
			FSMStateFunction m_onUpdateStateFunction;
			FSMStateFunction m_onExitStateFunction;

			ui32 m_stateID;
		};
	}
}
