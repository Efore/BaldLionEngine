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

		static void Update();

		static void QueueEvent(const EventEntry& e);

		static void RegisterEventHandler(const char* eventID, EventHandlerFunc eventHandlerFunc);
		static void UnregisterEventHandler(const char* eventID, EventHandlerFunc eventHandlerFunc);


	private:
		static void ProcessHandlerRegistrations();
		static void ProcessHandlerUnregistrations();

		struct EventHandlerEntry
		{
			EventHandlerEntry() = default;

			EventHandlerEntry(const char* eventID, const EventHandlerFunc& eventHandlerFunc) : eventID(eventID), eventHandler(eventHandlerFunc)
			{
			
			}

			const char* eventID;
			EventHandler eventHandler;			
		};


		static LockFreeStack<EventEntry, 256> s_eventStack;
		static LockFreeStack<EventHandlerEntry, 16> s_eventHandlerRegistrationStack;
		static LockFreeStack<EventHandlerEntry, 16> s_eventHandlerUnregistrationStack;

		static HashTable<StringId, DynamicArray<EventHandler>> s_handlerMap;


	};
}
