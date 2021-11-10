#include "blpch.h"
#include "MemoryManager.h"

namespace BaldLion
{
	namespace Memory
	{
		FreeListAllocator* MemoryManager::s_freeListMainAllocator;
		FreeListAllocator* MemoryManager::s_freeListRendererAllocator;		
		FreeListAllocator* MemoryManager::s_freeListECSAllocator;
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

			const size_t frameAllocatorSize = 400 * 1024 * 1024; //~400MB
			void* frameAllocatorStart = s_freeListMainAllocator->Allocate(frameAllocatorSize, __alignof(LinearAllocator));
			s_linearFrameAllocator = new (frameAllocatorStart) LinearAllocator("Linear Frame Allocator", frameAllocatorSize - sizeof(LinearAllocator), AddPointerOffset(frameAllocatorStart, sizeof(LinearAllocator)));

			const size_t stackAllocatorSize = 400 * 1024 * 1024; //~400MB
			void* stackAllocatorStart = s_freeListMainAllocator->Allocate(stackAllocatorSize, __alignof(StackAllocator));
			s_stackAllocator = new (stackAllocatorStart) StackAllocator("Stack Allocator", stackAllocatorSize - sizeof(StackAllocator), AddPointerOffset(stackAllocatorStart, sizeof(StackAllocator)));

			const size_t ecsSize = 200 * 1024 * 1024; //200MB
			void *ecsAllocatorStart = s_freeListMainAllocator->Allocate(ecsSize, __alignof(FreeListAllocator));
			s_freeListECSAllocator = new (ecsAllocatorStart) FreeListAllocator("Renderer FreeList Allocator", ecsSize - sizeof(FreeListAllocator), AddPointerOffset(ecsAllocatorStart, sizeof(FreeListAllocator)));

			const size_t rendererSize = 1024ULL * 1024 * 1024; //1GB
			void *rendererAllocatorStart = s_freeListMainAllocator->Allocate(rendererSize, __alignof(FreeListAllocator));
			s_freeListRendererAllocator = new (rendererAllocatorStart) FreeListAllocator("Renderer FreeList Allocator", rendererSize - sizeof(FreeListAllocator), AddPointerOffset(rendererAllocatorStart, sizeof(FreeListAllocator)));
			
		}

		void MemoryManager::Delete(AllocationType allocationType)
		{
			switch (allocationType)
			{
			case BaldLion::Memory::AllocationType::FreeList_Main:
				break;

			case BaldLion::Memory::AllocationType::Linear_Frame:
				if(s_linearFrameAllocator != nullptr)
					s_linearFrameAllocator->Delete();
				break;

			case BaldLion::Memory::AllocationType::Stack:
				if (s_stackAllocator != nullptr)
					s_stackAllocator->Delete();
				break;

			case BaldLion::Memory::AllocationType::FreeList_Renderer:
				break;

			case BaldLion::Memory::AllocationType::FreeList_ECS:
				break;

			default:
				break;
			}
		}

		void MemoryManager::Stop()
		{
			BL_LOG_CORE_INFO("Memory not freed:");

			for (std::unordered_map<void*, AllocationInfo>::const_iterator it = s_allocationMap.begin(); it != s_allocationMap.end(); ++it) {
				
				switch (it->second.allocationType)
				{
				case AllocationType::FreeList_Main:
					BL_LOG_CORE_INFO("FreeList Main Allocator");
					break;
				case AllocationType::FreeList_ECS:
					BL_LOG_CORE_INFO("FreeList ECS Allocator");
					break;
				case AllocationType::FreeList_Renderer:
					BL_LOG_CORE_INFO("FreeList Renderer Allocator");
					break;
				case AllocationType::Linear_Frame:
					BL_LOG_CORE_INFO("LinearFrame Allocator");
					break;
				case AllocationType::Stack:
					BL_LOG_CORE_INFO("Stack Allocator");
					break;

				default:
					break;
				}


				BL_LOG_CORE_INFO(STRINGID_TO_STRING(it->second.allocationDescription));				
				BL_LOG_CORE_INFO("Size: {0}",(it->second.allocationSize));
				BL_LOG_CORE_INFO("---------------");
			}

			if (s_linearFrameAllocator != nullptr)
			{
				s_linearFrameAllocator->Delete();
				Delete(s_linearFrameAllocator);
			}

			if (s_stackAllocator != nullptr)
			{		
				s_stackAllocator->Delete();
				Delete(s_stackAllocator);
			}

			if (s_freeListRendererAllocator != nullptr)
			{				
				s_freeListRendererAllocator->Delete();
				Delete(s_freeListRendererAllocator);
			}

			if (s_freeListECSAllocator != nullptr)
			{
				s_freeListECSAllocator->Delete();
				Delete(s_freeListECSAllocator);
			}

			s_freeListMainAllocator->Delete();
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

			case BaldLion::Memory::AllocationType::FreeList_ECS:
				return s_freeListECSAllocator;

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

			case BaldLion::Memory::AllocationType::FreeList_ECS:
				return s_freeListECSAllocator->Size();

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

			case BaldLion::Memory::AllocationType::FreeList_ECS:
				return s_freeListECSAllocator->GetUsedMemory();

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