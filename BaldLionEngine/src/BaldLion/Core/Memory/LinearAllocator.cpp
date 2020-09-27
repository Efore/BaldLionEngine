#include "blpch.h"
#include "LinearAllocator.h"
#include "MemoryUtils.h"

namespace BaldLion
{
	namespace Memory {

		LinearAllocator::LinearAllocator(size_t size, void* start) : Allocator(size, start), m_current_position(start)
		{

		}

		LinearAllocator::~LinearAllocator()
		{
			m_current_position = nullptr;
		}

		void* LinearAllocator::Allocate(size_t size, uint8_t alignment)
		{
			BL_ASSERT(size != 0 && alignment != 0, "Size and alignment must be bigger than 0");

			uint8_t adjustment = AlignForwardAdjustment(m_current_position, alignment);
			size_t totalSize = size + adjustment;

			if (m_used_memory + totalSize > m_size) 
				return nullptr;

			void* aligned_address = AddPointerOffset(m_current_position, adjustment);
			m_current_position = AddPointerOffset(aligned_address, size);
			m_used_memory += totalSize;
			m_num_allocations++;
			
			return aligned_address;
		}

		void LinearAllocator::Deallocate(void* p)
		{
			BL_ASSERT(false, "Use Clear() instead");
		}

		void LinearAllocator::Clear()
		{
			m_num_allocations = 0;
			m_used_memory = 0;
			m_current_position = m_start;			
		}
	}
}