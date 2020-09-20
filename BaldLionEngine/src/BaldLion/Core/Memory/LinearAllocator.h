#pragma once
#include "Allocator.h"


namespace BaldLion
{
	namespace Memory
	{
		class LinearAllocator : public Allocator {

		public:
			LinearAllocator(size_t size, void* start);
			~LinearAllocator();

			virtual void* Allocate(size_t size, uint8_t alignment) override;
			virtual void Deallocate(void* p) override;
			void Clear();

		private:

			LinearAllocator(const LinearAllocator&);

			//Prevent copies because it might cause errors 
			LinearAllocator& operator=(const LinearAllocator&);

		private:

			void* m_current_position;
		};
	}
}
