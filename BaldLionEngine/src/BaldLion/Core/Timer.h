#pragma once
#include <chrono>

namespace BaldLion
{
	template<typename Fn>
	class Timer
	{
	public:
		Timer(const char *name, Fn&& func)
			:m_name(name), m_stopped(false), m_resultFunction(func)
		{
			m_startTimePoint = std::chrono::high_resolution_clock::now();
		}

		~Timer()
		{
			if(!m_stopped)
				Stop();
		}	

		void Stop()
		{
			m_stopped = true;

			auto endTimepoint = std::chrono::high_resolution_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimePoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

			auto duration = end - start;
			double ms = duration * 0.001;

			m_resultFunction({ n_name, duration });
		}

	private:

		const char *m_name;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimePoint;
		bool m_stopped;
		Fn m_resultFunction;
	};
}
