#include "blpch.h"
#include "LinearAllocator.h"
#include "MemoryUtils.h"

namespace BaldLion
{
	namespace Memory {

		LinearAllocator::LinearAllocator(size_t size, void* start) : Allocator(size, start), m_current_pos(start)
		{

		}

		LinearAllocator::~LinearAllocator()
		{
			m_current_pos = nullptr;
		}

		void* LinearAllocator::Allocate(size_t size, uint8_t alignment)
		{
			uint8_t adjustment = AlignBackwardAdjustment(m_current_pos, alignment);
			size_t totalSize = size + adjustment;

			if (m_used_memory + totalSize > m_size) 
				return nullptr;

			void* aligned_address = AddPointerOffset(m_current_pos, adjustment);
			m_current_pos = AddPointerOffset(aligned_address, size);
			m_used_memory += totalSize;
			m_num_allocations++;
			
			return aligned_address;
		}

		void LinearAllocator::Deallocate(void* p)
		{
			BL_ASSERT(false, "Use Cclear() instead");
		}

		void LinearAllocator::Clear()
		{
			m_num_allocations = 0;
			m_used_memory = 0;
			m_current_pos = m_start;			
		}
	}
}