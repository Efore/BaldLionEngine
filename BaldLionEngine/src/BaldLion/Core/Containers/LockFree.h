#pragma once
#include "DynamicArray.h"

namespace BaldLion
{
	using namespace Memory;

	template <typename T, ui32 capacity> 
	class LockFreeStack
	{
	public:
		LockFreeStack() = default;

		LockFreeStack(Memory::AllocationType allocationType) : m_size(0)
		{			
			m_elements = MemoryManager::NewArray<T>("Dynamic array", allocationType, capacity);
		}

		LockFreeStack& operator= (const LockFreeStack& other)
		{
			if (&other == this)
				return *this;

			if (m_elements != nullptr)
			{
				for (ui32 i = 0; i < m_size; ++i)
				{
					m_elements[i].~T();
				}
				MemoryManager::DeleteNoDestructor(m_elements);
			}

			m_elements = other.m_elements;
			m_size.store(other.m_size);

			return *this;
		}

		~LockFreeStack()
		{

		}

		void Delete()
		{
			for (ui32 i = 0; i < m_size; ++i)
			{
				m_elements[i].~T();
			}
			MemoryManager::DeleteNoDestructor(m_elements);
			m_elements = nullptr;
		}

		template <typename... Args >
		T* EmplaceBack(Args&&... args);

		inline void PushBack(const T& element)
		{
			BL_ASSERT_LOG(m_size < capacity, "Capacity reached");
			const ui32 oldSize = m_size.fetch_add(1u, std::memory_order_relaxed);
			m_elements[oldSize] = element;
		}

		inline T PopBack()
		{
			const ui32 oldSize = m_size.fetch_sub(1u, std::memory_order_relaxed);
			return m_elements[oldSize - 1u];
		}

		inline void Resize(ui32 size)
		{
			BL_ASSERT_LOG(size <= Capacity, "Size cannot be bigger than capacity");

			// Free old elements
			if (size < m_size)
			{
				const ui32 elementsToFree = m_size - size;
				T* ptr = &m_elements[size];

				for (ui32 i = 0u; i < elementsToFree; ++i)
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

		inline bool IsFull() const 
		{
			return m_size == capacity;
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

		inline ui32 Capacity() const
		{
			return capacity;
		}

		inline ui32 Size() const
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

		inline T& operator[](ui32 index)
		{
			BL_ASSERT(index < m_size);
			return m_elements[index];
		}

		inline T& operator[](ui32 index) const
		{
			BL_ASSERT(index < m_size);
			return m_elements[index];
		}

		inline void Insert(const DynamicArray<T>& array)
		{	
			const ui32 oldSize =
				m_size.fetch_add(array.Size(), std::memory_order_relaxed);
			BL_ASSERT(oldSize + array.Size() <= Capacity);
			memcpy(&m_elements[oldSize], array.Data(), array.Size() * sizeof(T));
		}

		T* m_elements = nullptr;

	private:
		std::atomic<ui32> m_size;
	};

	template <typename T, ui32 capacity>
	template <typename... Args >
	T* LockFreeStack<T, capacity>::EmplaceBack(Args&&... args)
	{
		BL_ASSERT_LOG(m_size < capacity, "Capacity reached");
		const ui32 oldSize = m_size.fetch_add(1u, std::memory_order_relaxed);

		return new (&m_elements[oldSize]) T(std::forward<Args>(args)...);
	}

}