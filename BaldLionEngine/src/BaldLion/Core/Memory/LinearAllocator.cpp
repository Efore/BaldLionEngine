#include "blpch.h"
#include "LinearAllocator.h"
#include "MemoryUtils.h"

namespace BaldLion
{
	namespace Memory {

		LinearAllocator::LinearAllocator(const char* allocatorName, size_t size, void* start) : Allocator(allocatorName, size, start), m_current_position(start)
		{
			BL_ASSERT(size > 0, "Size must be bigger than 0");
		}

		LinearAllocator::~LinearAllocator()
		{
			m_current_position = nullptr;
		}

		void* LinearAllocator::Allocate(size_t size, uint8_t alignment)
		{
			const std::lock_guard<std::mutex> lock(m_mutex);

			BL_ASSERT(size != 0 && alignment != 0, "Size and alignment must be bigger than 0");

			uint8_t adjustment = AlignForwardAdjustment(m_current_position, alignment);
			size_t totalSize = size + adjustment;

			if (m_used_memory + totalSize > m_size)
			{
				BL_LOG_CORE_ERROR("{0}: Not enough memory", ID_TO_STRING(m_allocatorName));
				return nullptr;
			}

			void* aligned_address = AddPointerOffset(m_current_position, adjustment);
			m_current_position = AddPointerOffset(aligned_address, size);
			m_used_memory += totalSize;
			m_num_allocations++;
			
			return aligned_address;
		}

		void LinearAllocator::Deallocate(void* p)
		{
		}

		void LinearAllocator::Clear()
		{
			const std::lock_guard<std::mutex> lock(m_mutex);
			Allocator::Clear();
			m_current_position = m_start;			
		}
	}
}