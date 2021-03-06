#include "blpch.h"

namespace BaldLion
{
	namespace Memory
	{
		FreeListAllocator* MemoryManager::s_mainFreeListAllocator;
		FreeListAllocator* MemoryManager::s_rendererFreeListAllocator;
		LinearAllocator* MemoryManager::s_frameLinearAllocator;
		StackAllocator* MemoryManager::s_stackAllocator;
		
		std::unordered_map<void*, AllocationType> MemoryManager::s_allocationMap;
		std::unordered_map<void*, std::pair<StringId, size_t>> MemoryManager::s_allocationDescriptions;

		void* MemoryManager::s_memory;
		std::mutex MemoryManager::s_mutex;

		size_t MemoryManager::s_memorySize;

		void MemoryManager::Init(size_t memoryAllocationSize)
		{
			s_memorySize = 1024ULL * 1024 * 1024 * 2; //2GB
			s_memory = malloc(s_memorySize);

			if (s_memory == nullptr)
			{
				BL_LOG_CORE_ERROR ("Not enough memory! 2GB of RAM needed.");
				return;
			}

			s_mainFreeListAllocator = new (s_memory) FreeListAllocator("Main FreeList Allocator", s_memorySize - sizeof(FreeListAllocator), AddPointerOffset(s_memory, sizeof(FreeListAllocator)));

			size_t frameAllocatorSize = 400 * 1024 * 1024; //400MB
			void* frameAllocatorStart = s_mainFreeListAllocator->Allocate(frameAllocatorSize, __alignof(LinearAllocator));
			s_frameLinearAllocator = new (frameAllocatorStart) LinearAllocator("Linear Frame Allocator", frameAllocatorSize - sizeof(LinearAllocator), AddPointerOffset(frameAllocatorStart, sizeof(LinearAllocator)));

			size_t stackAllocatorSize = 400 * 1024 * 1024; //400MB
			void* stackAllocatorStart = s_mainFreeListAllocator->Allocate(stackAllocatorSize, __alignof(StackAllocator));
			s_stackAllocator = new (stackAllocatorStart) StackAllocator("Stack Allocator", stackAllocatorSize - sizeof(StackAllocator), AddPointerOffset(stackAllocatorStart, sizeof(StackAllocator)));

			size_t rendererSize = 1024ULL * 1024 * 1024; //1GB
			void *rendererAllocatorStart = s_mainFreeListAllocator->Allocate(rendererSize, __alignof(FreeListAllocator));
			s_rendererFreeListAllocator = new (rendererAllocatorStart) FreeListAllocator("Renderer FreeList Allocator", rendererSize - sizeof(FreeListAllocator), AddPointerOffset(rendererAllocatorStart, sizeof(FreeListAllocator)));
			
		}

		void MemoryManager::Clear(AllocationType allocationType)
		{
			switch (allocationType)
			{
			case BaldLion::Memory::AllocationType::FreeList_Main:
				break;

			case BaldLion::Memory::AllocationType::Linear_Frame:
				if(s_frameLinearAllocator != nullptr)
					s_frameLinearAllocator->Clear();
				break;

			case BaldLion::Memory::AllocationType::Stack:
				if (s_stackAllocator != nullptr)
					s_stackAllocator->Clear();
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

			if (s_stackAllocator != nullptr)
			{		
				s_stackAllocator->Clear();
				Delete(s_stackAllocator);
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

			case BaldLion::Memory::AllocationType::Stack:
				return s_stackAllocator;

			case BaldLion::Memory::AllocationType::FreeList_Renderer:
				return s_rendererFreeListAllocator;

			default:
				break;
			}

			return nullptr;
		}

		size_t MemoryManager::GetAllocatorSize(AllocationType allocationType)
		{
			switch (allocationType)
			{
			case BaldLion::Memory::AllocationType::FreeList_Main:
				return s_mainFreeListAllocator->Size();

			case BaldLion::Memory::AllocationType::Linear_Frame:
				return s_frameLinearAllocator->Size();

			case BaldLion::Memory::AllocationType::Stack:
				return s_stackAllocator->Size();

			case BaldLion::Memory::AllocationType::FreeList_Renderer:
				return s_rendererFreeListAllocator->Size();

			default:
				break;
			}

			return 0;
		}

		size_t MemoryManager::GetAllocatorUsedMemory(AllocationType allocationType)
		{
			switch (allocationType)
			{
			case BaldLion::Memory::AllocationType::FreeList_Main:
				return s_mainFreeListAllocator->GetUsedMemory();

			case BaldLion::Memory::AllocationType::Linear_Frame:
				return s_frameLinearAllocator->GetUsedMemory();

			case BaldLion::Memory::AllocationType::Stack:
				return s_stackAllocator->GetUsedMemory();

			case BaldLion::Memory::AllocationType::FreeList_Renderer:
				return s_rendererFreeListAllocator->GetUsedMemory();

			default:
				break;
			}

			return 0;
		}

		size_t MemoryManager::GetMemorySize()
		{
			return s_memorySize;
		}

	}
}