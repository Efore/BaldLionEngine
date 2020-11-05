#include "blpch.h"
#include "StackAllocator.h"
#include "MemoryUtils.h"

namespace BaldLion
{
	namespace Memory {

		StackAllocator::StackAllocator(size_t size, void* start) : Allocator(size, start), m_current_position(start), m_prev_position(nullptr)
		{
			BL_ASSERT(size > 0, "Size must be bigger than 0");
		}

		StackAllocator::~StackAllocator()
		{
			m_current_position = m_prev_position = nullptr;
		}

		void* StackAllocator::Allocate(size_t size, uint8_t alignment)
		{
			BL_ASSERT(size != 0 && alignment != 0, "Size and alignment must be bigger than 0");			

			uint8_t adjustment = AlignForwardAdjustmentWithHeader<AllocationHeader>(m_current_position, alignment);
			size_t totalSize = adjustment + size;

			if (m_used_memory + totalSize > m_size) return nullptr;

			void* aligned_address = AddPointerOffset(m_current_position, adjustment);

			AllocationHeader* header = (AllocationHeader*)SubstractPointerOffset(aligned_address, sizeof(AllocationHeader));
			header->adjustment = adjustment;
			header->prev_address = m_prev_position;

			m_prev_position = m_current_position;
			m_current_position = AddPointerOffset(m_current_position, totalSize);
			m_used_memory += totalSize;
			m_num_allocations++;

			return aligned_address;
		}

		void StackAllocator::Deallocate(void* p)
		{
			BL_ASSERT(p != nullptr, "p cannot be null");

			AllocationHeader* header = (AllocationHeader*)(SubstractPointerOffset(p, sizeof(AllocationHeader)));
			m_used_memory -= (uintptr_t)m_current_position - (uintptr_t)p + header->adjustment;

			m_current_position = SubstractPointerOffset(p, header->adjustment);
			m_prev_position = header->prev_address;

			m_num_allocations--;
		}

		void StackAllocator::Clear()
		{
			Allocator::Clear();
			m_current_position = m_start;
		}

	}
}


