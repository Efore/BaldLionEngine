#include "blpch.h"
#include "MemoryManager.h"

namespace BaldLion
{
	namespace Memory
	{

		FreeListAllocator* MemoryManager::s_mainAllocator;
		FreeListAllocator* MemoryManager::s_rendererAllocator;
		LinearAllocator* MemoryManager::s_tempAllocator;
		void* MemoryManager::s_memory;

		void MemoryManager::Init(size_t memoryAllocationSize)
		{
			size_t memory_size = 1024ULL * 1024 * 1024 * 2; //2GB
			s_memory = malloc(memory_size);

			if (s_memory == nullptr)
			{
				BL_LOG_CORE_ERROR ("Not enough memory! 2GB of RAM needed.");
				return;
			}

			s_mainAllocator = new (s_memory) FreeListAllocator(memory_size - sizeof(FreeListAllocator), AddPointerOffset(s_memory, sizeof(FreeListAllocator)));

			size_t tempAllocatorSize = 4 * 1024; //4MB
			void* tempAllocatorStart = s_mainAllocator->Allocate(tempAllocatorSize, __alignof(LinearAllocator));
			s_tempAllocator = new (tempAllocatorStart) LinearAllocator(tempAllocatorSize, tempAllocatorStart);

			size_t rendererSize = 1024ULL * 1024 * 1024; //1GB
			void *rendererAllocatorStart = s_mainAllocator->Allocate(rendererSize, __alignof(FreeListAllocator));
			s_rendererAllocator = new (rendererAllocatorStart) FreeListAllocator(rendererSize - sizeof(FreeListAllocator), AddPointerOffset(rendererAllocatorStart, sizeof(FreeListAllocator)));
		}

		void MemoryManager::Clear()
		{
			if (s_tempAllocator != nullptr)
			{
				s_tempAllocator->Clear();
				Delete(AllocationType::Main, s_tempAllocator);
			}

			if (s_rendererAllocator != nullptr)
			{
				Delete(AllocationType::Main, s_rendererAllocator);
			}

			s_mainAllocator->~FreeListAllocator();

			free(s_memory);			
		}


	}
}