#include "blpch.h"
#include "Allocator.h"


namespace BaldLion
{
	namespace Memory
	{
		Allocator::Allocator(const char* allocatorName, size_t size, void* start) : m_allocatorName(STRING_TO_ID(allocatorName)), m_size(size), m_start(start), m_num_allocations(0), m_used_memory(0)
		{
		}

		Allocator::~Allocator()
		{
			BL_ASSERT(m_num_allocations == 0 && m_used_memory == 0, "Memory Leak");
		}		

		void Allocator::Clear()
		{
			m_num_allocations = 0;
			m_used_memory = 0;
		}
	}
}