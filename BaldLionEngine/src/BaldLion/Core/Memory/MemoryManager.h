#pragma once
#include "LinearAllocator.h"
#include "StackAllocator.h"
#include "FreeListAllocator.h"


namespace BaldLion
{
	namespace Memory
	{
		enum class AllocationType {
			FrameScoped,
			Resource,

		};

		class MemoryManager {

		public:
			static void Init(size_t memoryAllocationSize); 
			static void* New(char* allocationName, AllocationType allocationType, size_t size);
			static void Delete(void* elementPointer);
		};
	}
}