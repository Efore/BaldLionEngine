#pragma once

#include "Recast.h"
#include "BaldLion/Core/Time.h"

namespace BaldLion::AI::Navigation {

	/// Recast build context.
	class BuildContext : public rcContext
	{
		Timer m_timers[RC_MAX_TIMERS];

		static const int MAX_MESSAGES = 1000;
		const char* m_messages[MAX_MESSAGES];
		int m_messageCount;
		static const int TEXT_POOL_SIZE = 8000;
		char m_textPool[TEXT_POOL_SIZE];
		int m_textPoolSize;

		bool m_initialized;

	public:
		BuildContext();

		void Init();
		void Stop();

		/// Dumps the log to stdout.
		void dumpLog(const char* format, ...);
		/// Returns number of log messages.
		int getLogCount() const;
		/// Returns log message text.
		const char* getLogText(const int i) const;

		bool GetInitialized() const;

	protected:
		/// Virtual functions for custom implementations.
		///@{
		virtual void doResetLog();
		virtual void doLog(const rcLogCategory category, const char* msg, const int len);
		virtual void doResetTimers();
		virtual void doStartTimer(const rcTimerLabel label);
		virtual void doStopTimer(const rcTimerLabel label);
		virtual int doGetAccumulatedTime(const rcTimerLabel label) const;
		///@}
	};


}