#pragma once
#include "BaldLion/Core/Memory/MemoryManager.h"

namespace BaldLion
{
	template <typename T, typename Allocator = Memory::Allocator>
	class BLVector
	{
	public:		
		BLVector();
		BLVector(Memory::AllocationType allocationType, size_t capacity);
		~BLVector();

		size_t Size() const { return m_size; }
		int Capacity() const { return m_capacity; }

		void Reserve(size_t capacity);	
		void Clear();

		template <typename... Args >
		void EmplaceBack(Args&&... args);

		void PushBack(const T& element);
		void PushBack(T&& element);

		void PopBack();

		void Remove(const T& element);
		void RemoveFast(const T& element);

		void RemoveAt(size_t index);
		void RemoveAtFast(size_t index);

		bool IsEmpty() const { return m_size == 0; }

		T& operator[](size_t index) { return m_elements[index]; }
		const T& operator[](size_t index) const { return m_elements[index]; }

	private:

		void Reallocate(size_t newCapacity);

	private:

		T* m_elements = nullptr;
		Allocator* m_allocator = nullptr;

		size_t m_size = 0;
		size_t m_capacity = 0;
	};

	template <typename T, typename Allocator>
	BaldLion::BLVector<T, Allocator>::BLVector()
	{
	}

	template <typename T, typename Allocator>
	BaldLion::BLVector<T, Allocator>::BLVector(Memory::AllocationType allocationType, size_t capacity) : m_allocator (Memory::MemoryManager::GetAllocator(allocationType)), m_size(0)
	{
		Reserve(capacity);
	}

	template <typename T, typename Allocator>
	BaldLion::BLVector<T, Allocator>::~BLVector()
	{
		Clear();
		m_allocator->Deallocate(m_elements);
		m_elements = nullptr;
	}

	template <typename T, typename Allocator>
	void BaldLion::BLVector<T, Allocator>::Reserve(size_t capacity)
	{
		m_capacity = capacity;
		m_elements = (T*)m_allocator->Allocate(capacity * sizeof(T), __alignof(T));
	}

	template <typename T, typename Allocator>
	void BaldLion::BLVector<T, Allocator>::Clear()
	{
		for (size_t i = 0; i < m_size; ++i)
		{
			m_elements[m_size].~T();
		}
		m_size = 0;
	}

	template <typename T, typename Allocator>
	template <typename... Args >
	void BaldLion::BLVector<T, Allocator>::EmplaceBack(Args&&... args)
	{
		if (m_size == m_capacity)
			Reallocate((size_t)(m_capacity * 1.5f));

		new (&m_elements[m_size]) T(std::forward<Args>(args)...);

		++m_size;
	}

	template <typename T, typename Allocator>
	void BaldLion::BLVector<T, Allocator>::PushBack(const T& element)
	{
		if (m_size == m_capacity)
			Reallocate((size_t)(m_capacity * 1.5f));

		m_elements[m_size] = element;
		++m_size;
	}

	template <typename T, typename Allocator>
	void BaldLion::BLVector<T, Allocator>::PushBack(T&& element)
	{
		if (m_size == m_capacity)
			Reallocate((size_t)(m_capacity * 1.5f));

		m_elements[m_size] = std::move(element);

		++m_size;
	}

	template <typename T, typename Allocator>
	void BaldLion::BLVector<T, Allocator>::PopBack()
	{		
		if (m_size == 0)
			return;

		--m_size;
		m_elements[m_size].~T();
	}

	template <typename T, typename Allocator>
	void BaldLion::BLVector<T, Allocator>::Remove(const T& element)
	{
		if (m_size == 0)
			return;

		size_t i = 0;
		for (; i <= m_size || m_elements[i] != element; ++i);

		RemoveAt(i);
	}

	template <typename T, typename Allocator>
	void BaldLion::BLVector<T, Allocator>::RemoveFast(const T& element)
	{
		if (m_size == 0)
			return;

		size_t i = 0;
		for (; i <= m_size || m_elements[i] != element; ++i);

		RemoveAtFast(i);
	}

	template <typename T, typename Allocator>
	void BaldLion::BLVector<T, Allocator>::RemoveAt(size_t index)
	{
		BL_ASSERT(index < m_size, "Index can't be bigger than or equal to m_size");			

		if (index == m_size - 1)
		{
			PopBack();
		}
		else
		{
			T* newLocation = (T*)m_allocator->Allocate(m_capacity * sizeof(T), __alignof(T));
			
			for (size_t i = 0; i < index; ++i)
			{
				newLocation[i] = std::move(m_elements[i]);
			}
			for (size_t i = index + 1; i < m_size; ++i)
			{
				newLocation[i - 1] = std::move(m_elements[i]);
			}

			for (size_t i = 0; i < m_size; ++i)
			{
				m_elements[m_size].~T();
			}

			m_allocator->Deallocate(m_elements);

			m_elements = newLocation;
			--m_size;
		}
	}

	template <typename T, typename Allocator>
	void BaldLion::BLVector<T, Allocator>::RemoveAtFast(size_t index)
	{
		BL_ASSERT(index < m_size, "Index can't be bigger than or equal to  m_size");

		if (index == m_size - 1)
		{
			PopBack();
		}
		else
		{
			--m_size;
			std::swap(m_elements[index], m_elements[m_size]);
			m_elements[m_size].~T();
		}
	}

	template <typename T, typename Allocator>
	void BaldLion::BLVector<T, Allocator>::Reallocate(size_t newCapacity)
	{
		T* newLocation = (T*)m_allocator->Allocate(newCapacity * sizeof(T), __alignof(T));

		const size_t newSize = newCapacity < m_size ? newCapacity : m_size;

		for (size_t i = 0; i < newSize; ++i)
		{
			newLocation[i] = std::move(m_elements[i]);
		}

		for (size_t i = 0; i < m_size; ++i)
		{
			m_elements[m_size].~T();
		}

		m_allocator->Deallocate(m_elements);

		m_elements = newLocation;
		m_capacity = newCapacity;
		m_size = newSize;
	}
}
