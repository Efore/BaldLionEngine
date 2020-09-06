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
			Main,
			Temp,
			Renderer,

		};

		class MemoryManager {

		public:
			static void Init(size_t memoryAllocationSize); 
			static void Clear();

			template <class T, class... Args >
			static T* New(char* allocationName, AllocationType allocationType, Args&&... args);

			template <class T>
			static void Delete(AllocationType allocationType, T* element);
		private:

			static FreeListAllocator* s_mainAllocator;
			static FreeListAllocator* s_rendererAllocator;
			static LinearAllocator* s_tempAllocator;
			static void* s_memory;
			//static std::map<void*, AllocationType> s_allocationMap;
		};

		template <class T, class... Args >
		T* MemoryManager::New(char* allocationName, AllocationType allocationType, Args&&... args)
		{
			T* result = nullptr;

			switch (allocationType)
			{
			case AllocationType::Main:
				result = new (s_mainAllocator->Allocate(sizeof(T), __alignof(T))) T(std::forward<Args>(args)...);
				break;
			case AllocationType::Temp:
				result = new (s_tempAllocator->Allocate(sizeof(T), __alignof(T))) T(std::forward<Args>(args)...);
				break;
			case AllocationType::Renderer:
				result = new (s_rendererAllocator->Allocate(sizeof(T), __alignof(T))) T(std::forward<Args>(args)...);
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
			case AllocationType::Main:
				s_mainAllocator->Deallocate(element);
				break;
			case AllocationType::Temp:
				s_tempAllocator->Deallocate(element);
				break;
			case AllocationType::Renderer:
				s_rendererAllocator->Deallocate(element);
				break;
			}
		}

	}
}