#pragma once
#include "Allocator.h"

namespace BaldLion
{
	namespace Memory
	{
		class FreeListAllocator : public Allocator {

		public:
			FreeListAllocator(size_t size, void* start);
			~FreeListAllocator();

			virtual void* Allocate(size_t size, uint8_t alignment) override;
			virtual void Deallocate(void* p) override;
			

		private:

			FreeListAllocator(const FreeListAllocator&);

			//Prevent copies because it might cause errors 
			FreeListAllocator& operator=(const FreeListAllocator&);

		private:

			struct AllocationHeader {
				size_t size;
				uint8_t adjustment;
			};

			struct AllocationBlock {
				size_t size;
				AllocationBlock* nextBlock;
			};

			AllocationBlock* m_allocationBlocksBegin;
		};
	}
}
