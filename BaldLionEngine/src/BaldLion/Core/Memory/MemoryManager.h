#pragma once
#include "MemoryUtils.h"
#include "LinearAllocator.h"
#include "StackAllocator.h"
#include "FreeListAllocator.h"
#include <map>

namespace BaldLion
{
	namespace Memory
	{
		enum class AllocationType {
			FreeList_Main,
			Linear_Frame,
			Stack_Temp,
			FreeList_Renderer,
		};

		class MemoryManager {

		public:
			static void Init(size_t memoryAllocationSize); 
			static void Clear(AllocationType allocationType);
			static void Clear();			

			template <class T, class... Args >
			static T* New(char* allocationName, AllocationType allocationType, Args&&... args);

			template <class T>
			static void Delete(AllocationType allocationType, T* element);

			static Allocator* GetAllocator(AllocationType allocationType);

		private:

			static FreeListAllocator* s_mainFreeListAllocator;
			static FreeListAllocator* s_rendererFreeListAllocator;
			static LinearAllocator* s_frameLinearAllocator;
			static StackAllocator* s_tempStackAllocator;
			static void* s_memory;
			//static std::map<void*, AllocationType> s_allocationMap;
		};

		template <class T, class... Args >
		T* MemoryManager::New(char* allocationName, AllocationType allocationType, Args&&... args)
		{
			T* result = nullptr;

			switch (allocationType)
			{
			case AllocationType::FreeList_Main:
				result = new (s_mainFreeListAllocator->Allocate(sizeof(T), __alignof(T))) T(std::forward<Args>(args)...);
				break;
			case AllocationType::Linear_Frame:
				result = new (s_frameLinearAllocator->Allocate(sizeof(T), __alignof(T))) T(std::forward<Args>(args)...);
				break;
			case AllocationType::Stack_Temp:
				result = new (s_tempStackAllocator->Allocate(sizeof(T), __alignof(T))) T(std::forward<Args>(args)...);
				break;
			case AllocationType::FreeList_Renderer:
				result = new (s_rendererFreeListAllocator->Allocate(sizeof(T), __alignof(T))) T(std::forward<Args>(args)...);
				break;
			}		

			return result;
			//s_allocationMap.emplace((void*)result, allocationType);
		}

		template <class T>
		void MemoryManager::Delete(AllocationType allocationType,T* element)
		{
			element->~T();
			
			switch (allocationType)
			{
			case AllocationType::FreeList_Main:
				s_mainFreeListAllocator->Deallocate(element);
				break;
			case AllocationType::Linear_Frame:
				s_frameLinearAllocator->Deallocate(element);
				break;
			case AllocationType::Stack_Temp:
				s_tempStackAllocator->Deallocate(element);
				break;
			case AllocationType::FreeList_Renderer:
				s_rendererFreeListAllocator->Deallocate(element);
				break;
			}
		}

	}
}