#include "blpch.h"
#include "EventManager.h"

namespace BaldLion
{
	HashTable<StringId, DynamicArray<EventHandler>> EventManager::s_handlerMap;
	LockFreeStack<EventEntry, 256> EventManager::s_eventStack;

	LockFreeStack<EventManager::EventHandlerEntry, 16> EventManager::s_eventHandlerRegistrationStack;
	LockFreeStack<EventManager::EventHandlerEntry, 16> EventManager::s_eventHandlerUnregistrationStack;

	void EventManager::Init()
	{
		s_handlerMap = HashTable<StringId, DynamicArray<EventHandler>>(AllocationType::FreeList_Main, 256);
		s_eventStack = LockFreeStack<EventEntry, 256>(AllocationType::FreeList_Main);
		s_eventHandlerRegistrationStack = LockFreeStack<EventHandlerEntry, 16>(AllocationType::FreeList_Main);
		s_eventHandlerUnregistrationStack = LockFreeStack<EventHandlerEntry, 16>(AllocationType::FreeList_Main);
	}

	void EventManager::Stop()
	{
		s_handlerMap.Delete();
		s_eventStack.Delete();
		s_eventHandlerRegistrationStack.Delete();
		s_eventHandlerUnregistrationStack.Delete();
	}

	void EventManager::RegisterEventHandler(const char* eventID, EventHandlerFunc eventHandlerFunc)
	{
		BL_ASSERT_LOG(!s_eventHandlerRegistrationStack.IsFull(), "HandlerRegistration Stack full, increase capacity.");		
		s_eventHandlerRegistrationStack.EmplaceBack(eventID, eventHandlerFunc);		
	}

	void EventManager::UnregisterEventHandler(const char* eventID, EventHandlerFunc eventHandlerFunc)
	{
		BL_ASSERT_LOG(!s_eventHandlerUnregistrationStack.IsFull(), "HandlerUnregistration Stack full, increase capacity.");
		s_eventHandlerUnregistrationStack.EmplaceBack(eventID, eventHandlerFunc);
	}

	void EventManager::ProcessHandlerRegistrations()
	{
		for (ui32 i = 0; i < s_eventHandlerRegistrationStack.Size(); ++i)
		{
			EventHandlerEntry& handlerRegistrationEntry = s_eventHandlerRegistrationStack[i];

			const StringId eventStringID = BL_STRING_TO_STRINGID(handlerRegistrationEntry.eventID);

			DynamicArray<EventHandler>* handlersForEvent = nullptr;
			if (s_handlerMap.TryGet(eventStringID, handlersForEvent))
			{
				bool handlerFound = false;

				BL_DYNAMICARRAY_FOREACH(*handlersForEvent)
				{
					if ((*handlersForEvent)[i] == handlerRegistrationEntry.eventHandler)
					{
						//If the event handler already exists, is not included
						handlerFound = true;
						break;
					}
				}

				if (!handlerFound)
				{
					handlersForEvent->EmplaceBack(handlerRegistrationEntry.eventHandler);
				}
			}
			else
			{
				s_handlerMap.Emplace(eventStringID, AllocationType::FreeList_Main, 16u);
				DynamicArray<EventHandler>* handlersForEvent = nullptr;
				if (s_handlerMap.TryGet(eventStringID, handlersForEvent))
				{
					handlersForEvent->EmplaceBack(handlerRegistrationEntry.eventHandler);
				}
			}
		}
		s_eventHandlerRegistrationStack.Clear();
	}

	void EventManager::ProcessHandlerUnregistrations()
	{
		for (ui32 i = 0; i < s_eventHandlerUnregistrationStack.Size(); ++i)
		{
			EventHandlerEntry& handlerUnregistrationEntry = s_eventHandlerUnregistrationStack[i];

			const StringId eventStringID = BL_STRING_TO_STRINGID(handlerUnregistrationEntry.eventID);

			DynamicArray<EventHandler>* handlersForEvent = nullptr;
			if (s_handlerMap.TryGet(eventStringID, handlersForEvent))
			{
				i32 handlerIndex = -1;

				BL_DYNAMICARRAY_FOREACH(*handlersForEvent)
				{
					if ((*handlersForEvent)[i] == handlerUnregistrationEntry.eventHandler)
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
		}
		s_eventHandlerUnregistrationStack.Clear();
	}

	void EventManager::Update()
	{
		ProcessHandlerRegistrations();

		//Dispatching events
		for (ui32 i = 0; i < s_eventStack.Size(); ++i)
		{
			EventEntry& eventEntry = s_eventStack[i];

			DynamicArray<EventHandler>* handlersForEvent = nullptr;
			if (s_handlerMap.TryGet(eventEntry.eventID, handlersForEvent))
			{				
				BL_DYNAMICARRAY_FOREACH(*handlersForEvent)
				{
					(*handlersForEvent)[i].eventHandlerFunc(eventEntry);
				}
			}
		}

		s_eventStack.Clear();

		ProcessHandlerUnregistrations();
	}

	void EventManager::QueueEvent(const EventEntry& e)
	{
		BL_ASSERT_LOG(!s_eventStack.IsFull(), "Event Stack full, increase capacity.");		
		s_eventStack.PushBack(e);		
	}

}