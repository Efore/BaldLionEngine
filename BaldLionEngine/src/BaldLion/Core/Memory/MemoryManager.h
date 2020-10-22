#pragma once
#include "MemoryUtils.h"
#include "LinearAllocator.h"
#include "StackAllocator.h"
#include "FreeListAllocator.h"
#include <unordered_map>

namespace BaldLion
{
	namespace Memory
	{
		enum class AllocationType {
			FreeList_Main,
			Linear_Frame,
			Stack_Scope_Temp,
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
			static void DeleteNoDestructor(T* element);
			template <class T>
			static void Delete(T* element);

			static Allocator* GetAllocator(AllocationType allocationType);

			template <class T>
			static AllocationType GetAllocatorType(const T* element);

		private:

			static FreeListAllocator* s_mainFreeListAllocator;
			static FreeListAllocator* s_rendererFreeListAllocator;
			static LinearAllocator* s_frameLinearAllocator;
			static StackAllocator* s_tempStackAllocator;
			static void* s_memory;
			static std::unordered_map<void*, AllocationType> s_allocationMap;
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
			case AllocationType::Stack_Scope_Temp:
				result = new (s_tempStackAllocator->Allocate(sizeof(T), __alignof(T))) T(std::forward<Args>(args)...);
				break;
			case AllocationType::FreeList_Renderer:
				result = new (s_rendererFreeListAllocator->Allocate(sizeof(T), __alignof(T))) T(std::forward<Args>(args)...);
				break;
			}		

			s_allocationMap.emplace((void*)result, allocationType);
			return result;
		}

		template <class T>
		void MemoryManager::DeleteNoDestructor(T* element)
		{
			BL_ASSERT(element != nullptr, "element cannot be null");

			if (s_allocationMap.find(element) == s_allocationMap.end())
				return;

			AllocationType allocationType = s_allocationMap[element];

			switch (allocationType)
			{
			case AllocationType::FreeList_Main:
				s_mainFreeListAllocator->Deallocate(element);
				break;
			case AllocationType::Linear_Frame:
				s_frameLinearAllocator->Deallocate(element);
				break;
			case AllocationType::Stack_Scope_Temp:
				s_tempStackAllocator->Deallocate(element);
				break;
			case AllocationType::FreeList_Renderer:
				s_rendererFreeListAllocator->Deallocate(element);
				break;
			}
		}
		
		template <class T>
		void MemoryManager::Delete(T* element)
		{
			BL_ASSERT(element != nullptr, "element cannot be null");

			element->~T();
			DeleteNoDestructor(element);
		}


		template <class T>
		AllocationType MemoryManager::GetAllocatorType(const T* element)
		{
			if (s_allocationMap.find(element) != s_allocationMap.end())
				return s_allocationMap[element];

			return AllocationType::FreeList_Main;
		}
	}
}