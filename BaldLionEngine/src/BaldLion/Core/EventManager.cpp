#include "blpch.h"
#include "EventManager.h"

namespace BaldLion
{
	std::mutex EventManager::s_handlerMapMutex;
	HashTable<StringId, DynamicArray<EventHandler>> EventManager::s_handlerMap;
	LockFreeStack<EventEntry, 256> EventManager::s_eventStack;

	void EventManager::Init()
	{
		s_handlerMap = HashTable<StringId, DynamicArray<EventHandler>>(AllocationType::FreeList_Main, 256);
		s_eventStack = LockFreeStack<EventEntry, 256>(AllocationType::FreeList_Main);
	}

	void EventManager::Stop()
	{
		s_handlerMap.Delete();
		s_eventStack.Delete();
	}

	void EventManager::RegisterHandler(const char* eventID, EventHandlerFunc eventHandlerFunc)
	{
		const StringId eventStringID = BL_STRING_TO_STRINGID(eventID);
		EventHandler eventHandler{ eventHandlerFunc };

		s_handlerMapMutex.lock();

		DynamicArray<EventHandler>* handlersForEvent = nullptr;
		if (s_handlerMap.TryGet(eventStringID, handlersForEvent))
		{
			bool handlerFound = false;

			BL_DYNAMICARRAY_FOREACH(*handlersForEvent)
			{
				if ((*handlersForEvent)[i] == eventHandler)
				{
					//If the event handler already exists, is not included
					handlerFound = true;
					break;
				}
			}
			
			if (!handlerFound)
			{
				handlersForEvent->EmplaceBack(eventHandler);
			}
		}
		else
		{
			s_handlerMap.Emplace(eventStringID, AllocationType::FreeList_Main, 16u);
			DynamicArray<EventHandler>* handlersForEvent = nullptr;
			if (s_handlerMap.TryGet(eventStringID, handlersForEvent))
			{
				handlersForEvent->EmplaceBack(eventHandler);
			}
		}

		s_handlerMapMutex.unlock();
	}

	void EventManager::UnregisterHandler(const char* eventID, EventHandlerFunc eventHandlerFunc)
	{
		const StringId eventStringID = BL_STRING_TO_STRINGID(eventID);
		EventHandler eventHandler{ eventHandlerFunc };

		s_handlerMapMutex.lock();

		DynamicArray<EventHandler>* handlersForEvent = nullptr;
		if (s_handlerMap.TryGet(eventStringID, handlersForEvent))
		{
			i32 handlerIndex = -1;

			BL_DYNAMICARRAY_FOREACH(*handlersForEvent)
			{
				if ((*handlersForEvent)[i] == eventHandler)
				{
					handlerIndex = i;
					break;
				}
			}

			if (handlerIndex >= 0)
			{
				handlersForEvent->RemoveAtFast(handlerIndex);
			}
		}

		s_handlerMapMutex.unlock();
	}

	void EventManager::DispatchEvents()
	{
		for (ui32 i = 0; i < s_eventStack.Size(); ++i)
		{
			EventEntry& eventEntry = s_eventStack[i];

			s_handlerMapMutex.lock();

			DynamicArray<EventHandler>* handlersForEvent = nullptr;
			if (s_handlerMap.TryGet(eventEntry.eventID, handlersForEvent))
			{
				//Dispatching events
				BL_DYNAMICARRAY_FOREACH(*handlersForEvent)
				{
					(*handlersForEvent)[i].eventHandlerFunc(eventEntry);
				}
			}

			s_handlerMapMutex.unlock();
		}

		s_eventStack.Clear();
	}

	void EventManager::QueueEvent(const EventEntry& e)
	{
		if (!s_eventStack.IsFull())
		{
			s_eventStack.PushBack(e);
		}
		else
		{
			BL_LOG_CORE_ERROR("Event Stack full, increase capacity.");
		}
	}

}