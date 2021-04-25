#include "blpch.h"
#include "MemoryManager.h"

namespace BaldLion
{
	namespace Memory
	{
		FreeListAllocator* MemoryManager::s_freeListMainAllocator;
		FreeListAllocator* MemoryManager::s_freeListRendererAllocator;
		LinearAllocator* MemoryManager::s_linearFrameAllocator;
		StackAllocator* MemoryManager::s_stackAllocator;
		
		std::unordered_map<void*, AllocationInfo> MemoryManager::s_allocationMap;

		void* MemoryManager::s_memory;
		std::mutex MemoryManager::s_mutex;
		std::mutex MemoryManager::s_arrayMutex;

		size_t MemoryManager::s_memorySize = 0;

		void MemoryManager::Init(size_t memoryAllocationSize)
		{
			if (s_memorySize > 0) return;

			s_memorySize = 1024ULL * 1024 * 1024 * 2; //2GB
			s_memory = malloc(s_memorySize);

			if (s_memory == nullptr)
			{
				BL_LOG_CORE_ERROR ("Not enough memory! 2GB of RAM needed.");
				return;
			}

			s_freeListMainAllocator = new (s_memory) FreeListAllocator("Main FreeList Allocator", s_memorySize - sizeof(FreeListAllocator), AddPointerOffset(s_memory, sizeof(FreeListAllocator)));

			size_t frameAllocatorSize = 400 * 1024 * 1024; //400MB
			void* frameAllocatorStart = s_freeListMainAllocator->Allocate(frameAllocatorSize, __alignof(LinearAllocator));
			s_linearFrameAllocator = new (frameAllocatorStart) LinearAllocator("Linear Frame Allocator", frameAllocatorSize - sizeof(LinearAllocator), AddPointerOffset(frameAllocatorStart, sizeof(LinearAllocator)));

			size_t stackAllocatorSize = 400 * 1024 * 1024; //400MB
			void* stackAllocatorStart = s_freeListMainAllocator->Allocate(stackAllocatorSize, __alignof(StackAllocator));
			s_stackAllocator = new (stackAllocatorStart) StackAllocator("Stack Allocator", stackAllocatorSize - sizeof(StackAllocator), AddPointerOffset(stackAllocatorStart, sizeof(StackAllocator)));

			size_t rendererSize = 1024ULL * 1024 * 1024; //1GB
			void *rendererAllocatorStart = s_freeListMainAllocator->Allocate(rendererSize, __alignof(FreeListAllocator));
			s_freeListRendererAllocator = new (rendererAllocatorStart) FreeListAllocator("Renderer FreeList Allocator", rendererSize - sizeof(FreeListAllocator), AddPointerOffset(rendererAllocatorStart, sizeof(FreeListAllocator)));
			
		}

		void MemoryManager::Clear(AllocationType allocationType)
		{
			switch (allocationType)
			{
			case BaldLion::Memory::AllocationType::FreeList_Main:
				break;

			case BaldLion::Memory::AllocationType::Linear_Frame:
				if(s_linearFrameAllocator != nullptr)
					s_linearFrameAllocator->Clear();
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
			if (s_linearFrameAllocator != nullptr)
			{
				s_linearFrameAllocator->Clear();
				Delete(s_linearFrameAllocator);
			}

			if (s_stackAllocator != nullptr)
			{		
				s_stackAllocator->Clear();
				Delete(s_stackAllocator);
			}

			if (s_freeListRendererAllocator != nullptr)
			{				
				s_freeListRendererAllocator->Clear();
				Delete(s_freeListRendererAllocator);
			}

			s_freeListMainAllocator->Clear();
			s_freeListMainAllocator->~FreeListAllocator();

			free(s_memory);			
		}

		Allocator* MemoryManager::GetAllocator(AllocationType allocationType)
		{
			switch (allocationType)
			{
			case BaldLion::Memory::AllocationType::FreeList_Main:
				return s_freeListMainAllocator;

			case BaldLion::Memory::AllocationType::Linear_Frame:
				return s_linearFrameAllocator;

			case BaldLion::Memory::AllocationType::Stack:
				return s_stackAllocator;

			case BaldLion::Memory::AllocationType::FreeList_Renderer:
				return s_freeListRendererAllocator;

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
				return s_freeListMainAllocator->Size();

			case BaldLion::Memory::AllocationType::Linear_Frame:
				return s_linearFrameAllocator->Size();

			case BaldLion::Memory::AllocationType::Stack:
				return s_stackAllocator->Size();

			case BaldLion::Memory::AllocationType::FreeList_Renderer:
				return s_freeListRendererAllocator->Size();

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
				return s_freeListMainAllocator->GetUsedMemory();

			case BaldLion::Memory::AllocationType::Linear_Frame:
				return s_linearFrameAllocator->GetUsedMemory();

			case BaldLion::Memory::AllocationType::Stack:
				return s_stackAllocator->GetUsedMemory();

			case BaldLion::Memory::AllocationType::FreeList_Renderer:
				return s_freeListRendererAllocator->GetUsedMemory();

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