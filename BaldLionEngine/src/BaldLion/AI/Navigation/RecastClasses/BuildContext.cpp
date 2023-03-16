#include "blpch.h"
#include "BuildContext.h"

namespace BaldLion::AI::Navigation
{
	BuildContext::BuildContext() : 
		m_messageCount(0),
		m_textPoolSize(0),
		m_initialized(false)
	{

	}

	void BuildContext::Init()
	{
		m_initialized = true;
		for (ui32 i = 0; i < RC_MAX_TIMERS; ++i)
		{
			Time::RequestNewTimer(m_timers[i]);
			m_timers[i].Pause();
		}
	}

	void BuildContext::Stop()
	{
		m_initialized = false;
		for (ui32 i = 0; i < RC_MAX_TIMERS; ++i)
		{
			Time::RemoveTimer(m_timers[i].GetTimerID());			
		}
	}

	void BuildContext::dumpLog(const char* format, ...)
	{
		// Print header.
		va_list ap;
		va_start(ap, format);
		vprintf(format, ap);
		va_end(ap);
		printf("\n");

		// Print messages
		const int TAB_STOPS[4] = { 28, 36, 44, 52 };
		for (int i = 0; i < m_messageCount; ++i)
		{
			const char* msg = m_messages[i] + 1;
			int n = 0;
			while (*msg)
			{
				if (*msg == '\t')
				{
					int count = 1;
					for (int j = 0; j < 4; ++j)
					{
						if (n < TAB_STOPS[j])
						{
							count = TAB_STOPS[j] - n;
							break;
						}
					}
					while (--count)
					{
						putchar(' ');
						n++;
					}
				}
				else
				{
					putchar(*msg);
					n++;
				}
				msg++;
			}
			putchar('\n');
		}
	}

	int BuildContext::getLogCount() const
	{
		return m_messageCount;
	}

	const char* BuildContext::getLogText(const int i) const
	{
		return m_messages[i] + 1;
	}

	void BuildContext::doResetLog()
	{
		m_messageCount = 0;
		m_textPoolSize = 0;
	}

	void BuildContext::doLog(const rcLogCategory category, const char* msg, const int len)
	{
		if (!len) return;
		if (m_messageCount >= MAX_MESSAGES)
			return;
		char* dst = &m_textPool[m_textPoolSize];
		int n = TEXT_POOL_SIZE - m_textPoolSize;
		if (n < 2)
			return;
		char* cat = dst;
		char* text = dst + 1;
		const int maxtext = n - 1;
		// Store category
		*cat = (char)category;
		// Store message
		const int count = rcMin(len + 1, maxtext);
		memcpy(text, msg, count);
		text[count - 1] = '\0';
		m_textPoolSize += 1 + count;
		m_messages[m_messageCount++] = dst;
	}

	void BuildContext::doResetTimers()
	{
		for (ui32 i = 0; i < RC_MAX_TIMERS; ++i)
		{
			m_timers[i].Reset();
		}
	}

	void BuildContext::doStartTimer(const rcTimerLabel label)
	{
		for (ui32 i = 0; i < RC_MAX_TIMERS; ++i)
		{			
			m_timers[i].Resume();
		}
	}

	void BuildContext::doStopTimer(const rcTimerLabel label)
	{
		for (ui32 i = 0; i < RC_MAX_TIMERS; ++i)
		{
			m_timers[i].Pause(); 
		}
	}

	int BuildContext::doGetAccumulatedTime(const rcTimerLabel label) const
	{
		return (int)(m_timers[label].GetCurrentTimeInMilliseconds() - m_timers[label].GetStartTimeInMilliseconds());
	}

	bool BuildContext::GetInitialized() const
	{	
		return m_initialized;		
	}
}
