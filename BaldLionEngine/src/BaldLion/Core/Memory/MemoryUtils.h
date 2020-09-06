#pragma once
#include "BaldLion/Core/Core.h"
#include "Allocator.h"

namespace BaldLion
{
	namespace Memory
	{
		template <class T> 
		T* AllocateArray(Allocator& allocator, size_t length)
		{
			BL_ASSERT(length != 0, "Length must be higher than 0");
			uint8_t headerSize = sizeof(size_t) / sizeof(T);

			if (sizeof(size_t) % sizeof(T) > 0) 
				headerSize += 1;

			//Allocate extra space to store array length in the bytes before the array 
			T* p = ((T*)allocator.Allocate(sizeof(T)*(length + headerSize), __alignof(T))) + headerSize;
			*(((size_t*)p) - 1) = length;

			for (size_t i = 0; i < length; i++)
				new (&p) T;

			return p;
		}

		template <class T>
		T* AllocateArrayWithoutConstruct(Allocator& allocator, size_t length)
		{
			BL_ASSERT(length != 0, "Length must be higher than 0");
			uint8_t headerSize = sizeof(size_t) / sizeof(T);

			if (sizeof(size_t) % sizeof(T) > 0) 
				headerSize += 1;

			//Allocate extra space to store array length in the bytes before the array 
			T* p = ((T*)allocator.Allocate(sizeof(T)*(length + headerSize), __alignof(T))) + headerSize;
			*(((size_t*)p) - 1) = length;

			return p;
		}

		template <class T> 
		void DeallocateArray(Allocator& allocator, T* array)
		{
			ASSERT(array != nullptr);
			size_t length = *(((size_t*)array) - 1);

			for (size_t i = 0; i < length; i++) 
				array.~T();

			//Calculate how much extra memory was allocated to store the length before the array 
			uint8_t headerSize = sizeof(size_t) / sizeof(T);

			if (sizeof(size_t) % sizeof(T) > 0)
				headerSize += 1;

			allocator.Deallocate(array - headerSize);
		}

		template <class T>
		void DeallocateArrayWithoutDestruct(Allocator& allocator, T* array)
		{
			ASSERT(array != nullptr);
			size_t length = *(((size_t*)array) - 1);

			//Calculate how much extra memory was allocated to store the length before the array 
			uint8_t headerSize = sizeof(size_t) / sizeof(T);

			if (sizeof(size_t) % sizeof(T) > 0)
				headerSize += 1;

			allocator.Deallocate(array - headerSize);
		}


		inline void* AlignForward(void* address, uint8_t alignment)
		{
			return (void*)((reinterpret_cast<uintptr_t>(address) + static_cast<uintptr_t>(alignment - 1)) & static_cast<uintptr_t>(~(alignment - 1)));
		}

		inline const void* AlignForward(const void* address, uint8_t alignment)
		{
			return (void*)((reinterpret_cast<uintptr_t>(address) + static_cast<uintptr_t>(alignment - 1)) & static_cast<uintptr_t>(~(alignment - 1)));
		}

		inline void* AlignBackward(void* address, uint8_t alignment)
		{
			return (void*)(reinterpret_cast<uintptr_t>(address)& static_cast<uintptr_t>(~(alignment - 1)));
		}

		inline const void* AlignBackward(const void* address, uint8_t alignment)
		{
			return (void*)(reinterpret_cast<uintptr_t>(address)& static_cast<uintptr_t>(~(alignment - 1)));
		}

		inline uint8_t AlignForwardAdjustment(const void* address, uint8_t alignment)
		{
			uint8_t adjustment = alignment - (reinterpret_cast<uintptr_t>(address)& static_cast<uintptr_t>(alignment - 1));

			if (adjustment == alignment)
				return 0; //already aligned

			return adjustment;
		}

		template<typename T>
		inline uint8_t AlignForwardAdjustmentWithHeader(const void* address, uint8_t alignment)
		{
			if (__alignof(T) > alignment)
				alignment = __alignof(T);

			uint8_t adjustment = sizeof(T) + AlignForwardAdjustment(AddPointerOffset(address, sizeof(T)), alignment);

			return adjustment;
		}

		inline uint8_t AlignBackwardAdjustment(const void* address, uint8_t alignment)
		{
			uint8_t adjustment = reinterpret_cast<uintptr_t>(address)& static_cast<uintptr_t>(alignment - 1);

			if (adjustment == alignment)
				return 0; //already aligned

			return adjustment;
		}

		inline void* AddPointerOffset(void* p, size_t x)
		{
			return (void*)(reinterpret_cast<uintptr_t>(p) + x);
		}

		inline const void* AddPointerOffset(const void* p, size_t x)
		{
			return (const void*)(reinterpret_cast<uintptr_t>(p) + x);
		}

		inline void* SubstractPointerOffset(void* p, size_t x)
		{
			return (void*)(reinterpret_cast<uintptr_t>(p) - x);
		}

		inline const void* SubstractPointerOffset(const void* p, size_t x)
		{
			return (const void*)(reinterpret_cast<uintptr_t>(p) - x);
		}

		inline bool IsAligned(const void* address, uint8_t alignment)
		{
			return AlignForwardAdjustment(address, alignment) == 0;
		}

		template<class T>
		inline bool IsAligned(const T* address)
		{
			return AlignForwardAdjustment(address, __alignof(T)) == 0;
		}
	}
}