#pragma once
#include "Allocator.h"


namespace BaldLion
{
	namespace Memory
	{
		class StackAllocator : public Allocator
		{
		public:
			StackAllocator(size_t size, void* start);
			~StackAllocator();

			virtual void* Allocate(size_t size, uint8_t alignment = 8) override;
			virtual void Deallocate(void* p) override;
			void Clear();

		private:

			struct AllocationHeader
			{
				void* prev_address;
				uint8_t adjustment;
			};

			StackAllocator (const StackAllocator&);
			StackAllocator& operator= (const StackAllocator&);

			void* m_prev_position;
			void* m_current_position;
		};
	}
}