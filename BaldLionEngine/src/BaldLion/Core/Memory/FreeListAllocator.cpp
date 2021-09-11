#include "blpch.h"
#include "FreeListAllocator.h"
#include "MemoryUtils.h"

namespace BaldLion
{
	namespace Memory {

		FreeListAllocator::FreeListAllocator(const char* allocatorName, size_t size, void* start) : Allocator(allocatorName, size, start), m_allocationBlocksBegin((AllocationBlock*)start)
		{
			BL_ASSERT(size > sizeof(FreeListAllocator::AllocationBlock), "Size must be bigger than size of AllocationBlock");

			m_allocationBlocksBegin->size = size;
			m_allocationBlocksBegin->nextBlock = nullptr;
		}

		FreeListAllocator::~FreeListAllocator()
		{
			m_allocationBlocksBegin = nullptr;
		}

		void* FreeListAllocator::Allocate(size_t size, uint8_t alignment)
		{
			BL_ASSERT(size != 0 && alignment != 0, "Size and alignment must be bigger than 0");

			const std::lock_guard<std::mutex> lock(m_mutex);

			AllocationBlock* bestFitBlock = nullptr;
			AllocationBlock* bestFitPrevBlock = nullptr;
			uint8_t bestFitAdjustment = 0;
			size_t bestFitTotalSize = 0;

			AllocationBlock* prevBlock = nullptr;
			AllocationBlock* currentBlock = m_allocationBlocksBegin;

			//Iterating through every block until a good fit is found
			while (currentBlock != nullptr)
			{
				//Calculate adjustment needed to keep object correctly aligned 
				uint8_t adjustment = AlignForwardAdjustmentWithHeader<AllocationHeader>(currentBlock, alignment);
				size_t totalSize = adjustment + size;
				
				//If it fits exactly in the size, we take that block and leave the loop
				if (currentBlock->size == totalSize)
				{
					bestFitPrevBlock = prevBlock;
					bestFitBlock = currentBlock;
					bestFitAdjustment = adjustment;
					bestFitTotalSize = totalSize;
					break;
				}

				//Otherwise keep searching the best fit
				if (currentBlock->size > totalSize && (bestFitBlock == nullptr || currentBlock->size < bestFitBlock->size))
				{
					bestFitPrevBlock = prevBlock;
					bestFitBlock = currentBlock;
					bestFitAdjustment = adjustment;
					bestFitTotalSize = totalSize;
				}

				prevBlock = currentBlock;
				currentBlock = currentBlock->nextBlock;
			}

			if (bestFitBlock == nullptr)
			{
				BL_LOG_CORE_ERROR("{0}: Not enough memory", STRINGID_TO_STRING(m_allocatorName));
				return nullptr;
			}


			//If the size of the current best fit block after allocating wont be enought to allocate more elements, 
			//it becomes full and "unnaccessible"
			if (bestFitBlock->size - bestFitTotalSize <= sizeof(AllocationHeader))
			{
				//Increase allocation size instead of creating a new FreeBlock
				bestFitTotalSize = bestFitBlock->size;

				if (bestFitPrevBlock != nullptr)
					bestFitPrevBlock->nextBlock = bestFitBlock->nextBlock;
				else
					m_allocationBlocksBegin = bestFitBlock->nextBlock;
			}
			//Otherwise, a new block is added after the current best fit block
			else
			{
				//Prevent new block from overwriting best fit block info
				BL_ASSERT(bestFitTotalSize > sizeof(AllocationBlock),"bestFitTotalSize must be bigger than size of AllocationBlock");

				// create a new FreeBlock containing remaining memory
				AllocationBlock* new_block = (AllocationBlock*)(AddPointerOffset(bestFitBlock, bestFitTotalSize));
				new_block->size = bestFitBlock->size - bestFitTotalSize;
				new_block->nextBlock = bestFitBlock->nextBlock;


				if (bestFitPrevBlock != nullptr)
					bestFitPrevBlock->nextBlock = new_block;
				else
					m_allocationBlocksBegin = new_block;
			}

			void* alignedResult = AddPointerOffset(bestFitBlock, bestFitAdjustment);
			
			AllocationHeader* header = (AllocationHeader*)SubstractPointerOffset(alignedResult, sizeof(AllocationHeader));
			header->size = bestFitTotalSize;
			header->adjustment = bestFitAdjustment;
			
			BL_ASSERT(IsAligned(header), "Header must be aligned");

			m_used_memory += bestFitTotalSize;
			m_num_allocations++; 

			return alignedResult;
		}

		void FreeListAllocator::Deallocate(void* p)
		{
			BL_ASSERT(p != nullptr, "p cannot be null");

			const std::lock_guard<std::mutex> lock(m_mutex);

			//Getting the header of the element to deallocate
			AllocationHeader* header = (AllocationHeader*)SubstractPointerOffset(p, sizeof(AllocationHeader));

			//Getting the element's block information
			void* blockStart = SubstractPointerOffset(p, header->adjustment);
			size_t blockSize = header->size;
			void* blockEnd = AddPointerOffset(blockStart,blockSize);

			AllocationBlock* prevFreeBlock = nullptr;
			AllocationBlock* currentFreeBlock = m_allocationBlocksBegin;

			//Iterating until reaching the deallocating block
			while (currentFreeBlock != nullptr)
			{
				if((void*)currentFreeBlock >= blockEnd)
					break;

				prevFreeBlock = currentFreeBlock;
				currentFreeBlock = currentFreeBlock->nextBlock;
			}

			//If the deallocating block is the first one in the list, we make access to it
			if (prevFreeBlock == nullptr)
			{
				prevFreeBlock = (AllocationBlock*)blockStart;
				prevFreeBlock->size = blockSize;
				prevFreeBlock->nextBlock = m_allocationBlocksBegin;
			}
			else if (AddPointerOffset(prevFreeBlock, prevFreeBlock->size) == blockStart)
			{
				prevFreeBlock->size += blockSize;
			}
			else
			{
				AllocationBlock* temp = (AllocationBlock*)blockStart;
				temp->size = blockSize;
				temp->nextBlock = prevFreeBlock->nextBlock;

				prevFreeBlock->nextBlock = temp;
				prevFreeBlock = temp;
			}

			//assert prevFreeBlock = nullptr

			if (AddPointerOffset(prevFreeBlock, prevFreeBlock->size) == prevFreeBlock->nextBlock)
			{
				prevFreeBlock->size += prevFreeBlock->nextBlock->size;
				prevFreeBlock->nextBlock = prevFreeBlock->nextBlock->nextBlock;
			}

			m_num_allocations--;
			m_used_memory -= blockSize;
		}

		void FreeListAllocator::Delete()
		{
			const std::lock_guard<std::mutex> lock(m_mutex);
			Allocator::Delete();
			m_allocationBlocksBegin = nullptr;
		}

	}
}