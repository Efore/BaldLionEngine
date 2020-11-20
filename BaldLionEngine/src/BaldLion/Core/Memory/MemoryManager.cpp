#include "blpch.h"

namespace BaldLion
{
	namespace Memory
	{

		FreeListAllocator* MemoryManager::s_mainFreeListAllocator;
		FreeListAllocator* MemoryManager::s_rendererFreeListAllocator;
		LinearAllocator* MemoryManager::s_frameLinearAllocator;
		StackAllocator* MemoryManager::s_tempStackAllocator;
		
		std::unordered_map<void*, AllocationType> MemoryManager::s_allocationMap;

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

			s_mainFreeListAllocator = new (s_memory) FreeListAllocator(memory_size - sizeof(FreeListAllocator), AddPointerOffset(s_memory, sizeof(FreeListAllocator)));

			size_t frameAllocatorSize = 4 * 1024 * 1024; //4MB
			void* frameAllocatorStart = s_mainFreeListAllocator->Allocate(frameAllocatorSize, __alignof(LinearAllocator));
			s_frameLinearAllocator = new (frameAllocatorStart) LinearAllocator(frameAllocatorSize - sizeof(LinearAllocator), AddPointerOffset(frameAllocatorStart, sizeof(LinearAllocator)));

			size_t stackAllocatorSize = 4 * 1024 * 1024; //4MB
			void* stackAllocatorStart = s_mainFreeListAllocator->Allocate(stackAllocatorSize, __alignof(StackAllocator));
			s_tempStackAllocator = new (stackAllocatorStart) StackAllocator(stackAllocatorSize - sizeof(StackAllocator), AddPointerOffset(stackAllocatorStart, sizeof(StackAllocator)));

			size_t rendererSize = 1024ULL * 1024 * 1024; //1GB
			void *rendererAllocatorStart = s_mainFreeListAllocator->Allocate(rendererSize, __alignof(FreeListAllocator));
			s_rendererFreeListAllocator = new (rendererAllocatorStart) FreeListAllocator(rendererSize - sizeof(FreeListAllocator), AddPointerOffset(rendererAllocatorStart, sizeof(FreeListAllocator)));
			
		}

		void MemoryManager::Stop(AllocationType allocationType)
		{
			switch (allocationType)
			{
			case BaldLion::Memory::AllocationType::FreeList_Main:
				break;

			case BaldLion::Memory::AllocationType::Linear_Frame:
				if(s_frameLinearAllocator != nullptr)
					s_frameLinearAllocator->Clear();
				break;

			case BaldLion::Memory::AllocationType::Stack_Scope_Temp:
				if (s_tempStackAllocator != nullptr)
					s_tempStackAllocator->Clear();
				break;

			case BaldLion::Memory::AllocationType::FreeList_Renderer:
				break;

			default:
				break;
			}
		}

		void MemoryManager::Stop()
		{
			if (s_frameLinearAllocator != nullptr)
			{
				s_frameLinearAllocator->Clear();
				Delete(s_frameLinearAllocator);
			}

			if (s_tempStackAllocator != nullptr)
			{		
				s_tempStackAllocator->Clear();
				Delete(s_tempStackAllocator);
			}

			if (s_rendererFreeListAllocator != nullptr)
			{				
				s_rendererFreeListAllocator->Clear();
				Delete(s_rendererFreeListAllocator);
			}

			s_mainFreeListAllocator->Clear();
			s_mainFreeListAllocator->~FreeListAllocator();

			free(s_memory);			
		}

		Allocator* MemoryManager::GetAllocator(AllocationType allocationType)
		{
			switch (allocationType)
			{
			case BaldLion::Memory::AllocationType::FreeList_Main:
				return s_mainFreeListAllocator;

			case BaldLion::Memory::AllocationType::Linear_Frame:
				return s_frameLinearAllocator;

			case BaldLion::Memory::AllocationType::Stack_Scope_Temp:
				return s_tempStackAllocator;

			case BaldLion::Memory::AllocationType::FreeList_Renderer:
				return s_rendererFreeListAllocator;

			default:
				break;
			}

			return nullptr;
		}

	}
}