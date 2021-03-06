#pragma once

#include <memory>
#include <mutex>
#include "BaldLion/Core/StringId.h"

namespace BaldLion
{
	namespace Memory
	{
		class Allocator
		{
		public:
			Allocator(const char* allocatorName, size_t size, void* start);

			virtual ~Allocator();

			virtual void* Allocate(size_t size, uint8_t alignment = 8) = 0;
			virtual void Deallocate(void* p) = 0;
			virtual void Clear();

			void* GetStart() { return m_start; }
			size_t Size() const { return m_size; }
			size_t GetUsedMemory() const { return m_used_memory; }
			size_t GetNumAllocations() const { return m_num_allocations; }

		protected:

			Allocator (const Allocator&);
			Allocator& operator= (const Allocator&);

			size_t m_size;
			size_t m_used_memory;
			size_t m_num_allocations;

			std::mutex m_mutex;
			void* m_start;

			StringId m_allocatorName;
		};
	}
}
