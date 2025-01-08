#pragma once
#include "DynamicArray.h"

namespace BaldLion
{
	using namespace Memory;

	template <typename T, uint64_t Capacity> 
	class LockFreeStack
	{
	public:
		LockFreeStack() = default;

		LockFreeStack(Memory::AllocationType allocationType)
		{
			m_elements = MemoryManager::NewArray<T>("Dynamic array", allocationType, Capacity);
			m_capacity = Capacity;
			m_size = 0u;
		}

		~LockFreeStack()
		{
			for (ui32 i = 0; i < m_size; ++i)
			{
				m_elements[i].~T();
			}
			MemoryManager::DeleteNoDestructor(m_elements);
			m_elements = nullptr;
		}

		inline void PushBack(const T& element)
		{
			const uint64_t oldSize = m_size.fetch_add(1u, std::memory_order_relaxed);
			m_elements[oldSize] = element;
		}

		inline T PopBack()
		{
			const uint64_t oldSize = m_size.fetch_sub(1u, std::memory_order_relaxed);
			return m_elements[oldSize - 1u];
		}

		inline void Resize(uint64_t size)
		{
			BL_ASSERT_LOG(size <= Capacity, "Size cannot be bigger than capacity");

			// Free old elements
			if (size < m_size)
			{
				const uint64_t elementsToFree = m_size - size;
				T* ptr = &m_elements[size];

				for (uint64_t i = 0u; i < elementsToFree; ++i)
					(ptr++)->~T();
			}

			m_size = size;
		}

		inline void Clear()
		{
			m_size = 0;
		}

		inline bool Empty() const
		{
			return m_size == 0u;
		}

		inline T& Back()
		{
			BL_ASSERT(!Empty());
			return m_elements[m_size - 1u];
		}

		inline const T& Back() const
		{
			BL_ASSERT(!Empty());
			return m_elements[m_size - 1u];
		}

		inline uint64_t Capacity() const
		{
			return m_capacity;
		}

		inline uint64_t Size() const
		{
			return m_size;
		}

		inline T* Begin()
		{
			return &m_elements[0];
		}

		inline const T* Begin() const
		{
			return &m_elements[0];
		}

		inline T* End()
		{
			return &m_elements[m_size];
		}

		inline const T* End() const
		{
			return &m_elements[m_size];
		}

		inline T& operator[](uint64_t index)
		{
			BL_ASSERT(index < m_size);
			return m_elements[index];
		}

		inline T& operator[](uint64_t index) const
		{
			BL_ASSERT(index < m_size);
			return m_elements[index];
		}

		inline void Insert(const DynamicArray<T>& array)
		{
			const uint64_t oldSize =
				m_size.fetch_add(array.Size(), std::memory_order_relaxed);
			BL_ASSERT(oldSize + array.Size() <= m_capacity);
			memcpy(&m_elements[oldSize], array.Data(), array.Size() * sizeof(T));
		}

		T* m_elements = nullptr;

	private:
		uint64_t m_capacity;
		std::atomic<uint64_t> m_size;
	};
}