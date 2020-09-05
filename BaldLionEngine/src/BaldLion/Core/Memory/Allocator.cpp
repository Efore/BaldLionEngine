#include "blpch.h"
#include "Allocator.h"


namespace BaldLion
{
	namespace Memory
	{
		Allocator::Allocator(size_t size, void* start) : m_size(size), m_start(start), m_num_allocations(0), m_used_memory(0)
		{
		}

		Allocator::~Allocator()
		{
			BL_ASSERT(m_num_allocations == 0 && m_used_memory == 0, "Memory Leak");
		}		
	}
}