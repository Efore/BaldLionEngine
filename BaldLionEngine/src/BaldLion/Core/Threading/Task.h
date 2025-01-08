#pragma once

#include "BaldLion/Core/Containers/DynamicArray.h"
#include "BaldLion/Core/StringId.h"
#include <atomic>
#include <mutex>

namespace BaldLion
{
	namespace Threading
	{
		typedef std::function<void()> OnTaskFinishedCallback;

		struct Task
		{
			inline void Wait() const
			{
				while (counter > 0);
			}

			OnTaskFinishedCallback callback;
			std::atomic<ui32> counter;
		};
	}
}
