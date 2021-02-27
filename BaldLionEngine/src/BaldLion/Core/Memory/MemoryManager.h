#pragma once
#include "MemoryUtils.h"
#include "LinearAllocator.h"
#include "StackAllocator.h"
#include "FreeListAllocator.h"
#include "BaldLion/Core/StringId.h"
#include <unordered_map>
#include <mutex>

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
			static void Stop(AllocationType allocationType);
			static void Stop();			

			template <class T, class... Args >
			static T* New(StringId allocationName, AllocationType allocationType, Args&&... args);

			template <class T>
			static void DeleteNoDestructor(T* element);
			template <class T>
			static void Delete(T* element);

			static Allocator* GetAllocator(AllocationType allocationType);

			static size_t GetAllocatorSize(AllocationType allocationType);
			static size_t GetAllocatorUsedMemory(AllocationType allocationType);

			template <class T>
			static AllocationType GetAllocatorType(const T* element);

			static size_t GetMemorySize();

		private:

			static FreeListAllocator* s_mainFreeListAllocator;
			static FreeListAllocator* s_rendererFreeListAllocator;
			static LinearAllocator* s_frameLinearAllocator;
			static StackAllocator* s_tempStackAllocator;
			static void* s_memory;
			static std::unordered_map<void*, AllocationType> s_allocationMap;
			static std::unordered_map<void*, StringId> s_allocationDescriptions;
			static std::mutex s_mutex;
			static size_t s_memorySize;
		};

		template <class T, class... Args >
		T* MemoryManager::New(StringId allocationName, AllocationType allocationType, Args&&... args)
		{
			const std::lock_guard<std::mutex> lock(s_mutex);

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
			s_allocationDescriptions.emplace((void*)result, allocationName);

			return result;
		}

		template <class T>
		void MemoryManager::DeleteNoDestructor(T* element)
		{
			const std::lock_guard<std::mutex> lock(s_mutex);

			BL_ASSERT(element != nullptr, "element cannot be null");

			if (s_allocationMap.find(element) == s_allocationMap.end())
				return;

			AllocationType allocationType = s_allocationMap[element];

			switch (allocationType)
			{
			case AllocationType::FreeList_Main:
				if(s_mainFreeListAllocator != nullptr)
					s_mainFreeListAllocator->Deallocate(element);
				break;
			case AllocationType::Linear_Frame:
				if (s_mainFreeListAllocator != nullptr)
					s_mainFreeListAllocator->Deallocate(element);
				break;
			case AllocationType::Stack_Scope_Temp:
				if (s_mainFreeListAllocator != nullptr)
					s_mainFreeListAllocator->Deallocate(element);
				break;
			case AllocationType::FreeList_Renderer:
				if (s_mainFreeListAllocator != nullptr)
					s_mainFreeListAllocator->Deallocate(element);
				break;
			}

			s_allocationMap.erase(element);
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