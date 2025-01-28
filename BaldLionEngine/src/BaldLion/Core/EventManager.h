#pragma once
#include "BaldLion/Core/Containers/LockFree.h"
#include "BaldLion/Core/Containers/HashTable.h"
#include "BaldLion/Core/Variant.h"
#include <functional>
#include <mutex>

namespace BaldLion
{
	struct EventEntry
	{
		StringId eventID;
		StringId senderID;		
		Variant eventData1;
		Variant eventData2;
		Variant eventData3;
	};

	typedef std::function<bool(const EventEntry& e)> EventHandlerFunc;

	struct EventHandler
	{
		EventHandlerFunc eventHandlerFunc;

		bool operator==(const EventHandler& other) const
		{
			return eventHandlerFunc.target<EventHandlerFunc>() == other.eventHandlerFunc.target<EventHandlerFunc>();
		}
	};
	
	class EventManager
	{
	public:		

		static void Init();
		static void Stop();

		static void DispatchEvents();

		static void QueueEvent(const EventEntry& e);

		static void RegisterHandler(const char* eventID, EventHandlerFunc eventHandlerFunc);
		static void UnregisterHandler(const char* eventID, EventHandlerFunc eventHandlerFunc);

	private:

		static LockFreeStack<EventEntry, 256> s_eventStack;

		static HashTable<StringId, DynamicArray<EventHandler>> s_handlerMap;

		static std::mutex s_handlerMapMutex;

	};
}
