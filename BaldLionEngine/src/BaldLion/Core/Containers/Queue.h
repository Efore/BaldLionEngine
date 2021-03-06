#pragma once
#include "BaldLion/Core/Memory/MemoryManager.h"

namespace BaldLion
{
	template <typename T, typename Allocator = Memory::Allocator>
	class Queue
	{
	public:
		Queue();
		Queue(Memory::AllocationType allocationType, ui32 capacity);
		Queue(Memory::AllocationType allocationType, const Queue<T, Allocator>& other);
		Queue(const Queue<T, Allocator>& other);
		Queue(Queue<T, Allocator>&& other) noexcept;

		~Queue();

		ui32 Size() const { return m_size - m_frontIndex; }
		ui32 Capacity() const { return m_capacity; }

		void Clear();
		void ClearNoDestructor();

		template <typename... Args >
		void EmplaceBack(Args&&... args);

		void PushBack(const T& element);
		void PushBack(T&& element);

		T& Front();
		const T& Front() const;

		void Pop();

		bool Exists(const T& element);

		bool IsEmpty() const { return m_size - m_frontIndex == 0; }

		Queue<T, Allocator>& operator= (const Queue<T, Allocator>& other);
		Queue<T, Allocator>& operator= (Queue<T, Allocator>&& other) noexcept;

	private:

		void Reserve(ui32 capacity);
		void Reallocate(ui32 newCapacity);

	private:

		T* m_elements = nullptr;
		Allocator* m_allocator = nullptr;

		ui32 m_frontIndex = 0;
		ui32 m_size = 0;
		ui32 m_capacity = 0;
	};

	template <typename T, typename Allocator>
	BaldLion::Queue<T, Allocator>::Queue()
	{
	}

	template <typename T, typename Allocator>
	BaldLion::Queue<T, Allocator>::Queue(Memory::AllocationType allocationType, ui32 capacity)
		: m_allocator(Memory::MemoryManager::GetAllocator(allocationType)), m_size(0)
	{
		Reserve(capacity);
	}

	template <typename T, typename Allocator>
	BaldLion::Queue<T, Allocator>::Queue(Memory::AllocationType allocationType, const Queue<T, Allocator>& other)
		: m_allocator(Memory::MemoryManager::GetAllocator(allocationType)), m_size(other.Size()), m_frontIndex(other.m_frontIndex)
	{
		Reserve(other.m_capacity);

		std::copy(other.m_elements, other.m_elements + m_size, m_elements);
	}

	template <typename T, typename Allocator>
	BaldLion::Queue<T, Allocator>::Queue(const Queue<T, Allocator>& other)
		: m_allocator(other.m_allocator), m_size(other.Size()), m_frontIndex(other.m_frontIndex)
	{
		Reserve(other.m_capacity);
		std::copy(other.m_elements, other.m_elements + m_size, m_elements);
	}

	template <typename T, typename Allocator>
	BaldLion::Queue<T, Allocator>::Queue(Queue<T, Allocator>&& other) noexcept
		: m_allocator(other.m_allocator), m_size(other.Size()), m_capacity(other.m_capacity), m_frontIndex(other.m_frontIndex)
	{
		m_elements = other.m_elements;
		
		other.m_elements = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
	}

	template <typename T, typename Allocator>
	Queue<T, Allocator>& BaldLion::Queue<T, Allocator>::operator=(const Queue<T, Allocator>& other)
	{		
		if (&other == this)
			return *this;

		m_allocator = other.m_allocator;
		m_size = other.m_size;
		m_frontIndex = other.m_frontIndex;
		Reserve(other.Capacity());

		std::copy(other.m_elements, other.m_elements + m_size, m_elements);

		return *this;
	}

	template <typename T, typename Allocator>
	Queue<T, Allocator>& BaldLion::Queue<T, Allocator>::operator=(Queue<T, Allocator>&& other) noexcept
	{
		if (&other == this)
			return *this;

		Clear();

		m_allocator = other.m_allocator;
		m_size = other.m_size;
		m_capacity = other.m_capacity;
		m_elements = other.m_elements;

		other.m_elements = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
		
		return *this;
	}

	template <typename T, typename Allocator>
	BaldLion::Queue<T, Allocator>::~Queue()
	{
		
	}
	
	template <typename T, typename Allocator>
	void BaldLion::Queue<T, Allocator>::Clear()
	{
		if (m_elements == nullptr)
		{
			return;
		}

		for (ui32 i = 0; i < m_size; ++i)
		{
			m_elements[m_size].~T();
		}

		ClearNoDestructor();
	}

	template <typename T, typename Allocator>
	void BaldLion::Queue<T, Allocator>::ClearNoDestructor()
	{
		if (m_elements == nullptr)
		{
			return;
		}

		m_size = 0;
		m_frontIndex = 0;
		m_allocator->Deallocate(m_elements);
		m_elements = nullptr;
	}

	template <typename T, typename Allocator>
	template <typename... Args >
	void BaldLion::Queue<T, Allocator>::EmplaceBack(Args&&... args)
	{
		BL_ASSERT(m_capacity > 0, "Capacity is 0");

		if (m_size == m_capacity)
		{
			Reallocate((ui32)(m_capacity * 1.5f));
		}

		new (&m_elements[m_size++]) T(std::forward<Args>(args)...);
	}

	template <typename T, typename Allocator>
	void BaldLion::Queue<T, Allocator>::PushBack(const T& element)
	{
		BL_ASSERT(m_capacity > 0, "Capacity is 0");

		if (m_size == m_capacity)
		{
			Reallocate((ui32)(m_capacity * 1.5f));
		}

		m_elements[m_size++] = element;
	}

	template <typename T, typename Allocator>
	void BaldLion::Queue<T, Allocator>::PushBack(T&& element)
	{
		BL_ASSERT(m_capacity > 0, "Capacity is 0");

		if (m_size == m_capacity)
		{
			Reallocate((ui32)(m_capacity * 1.5f));
		}

		m_elements[m_size++] = std::move(element);
	}

	template <typename T, typename Allocator>
	bool BaldLion::Queue<T, Allocator>::Exists(const T& element)
	{
		return FindIndex(element) != -1;
	}
	
	template <typename T, typename Allocator /*= Memory::Allocator*/>
	T& BaldLion::Queue<T, Allocator>::Front()
	{
		BL_ASSERT(m_size > 0, "Size is 0");		
		return m_elements[m_frontIndex];
	}

	template <typename T, typename Allocator /*= Memory::Allocator*/>
	const T& BaldLion::Queue<T, Allocator>::Front() const
	{
		BL_ASSERT(m_size > 0, "Size is 0");
		return m_elements[m_frontIndex];
	}

	template <typename T, typename Allocator /*= Memory::Allocator*/>
	void BaldLion::Queue<T, Allocator>::Pop()
	{
		BL_ASSERT(m_size > 0, "Size is 0");

		if (++m_frontIndex == m_size)
		{
			m_frontIndex = 0;
			m_size = 0;
		}
	}

	template <typename T, typename Allocator>
	void BaldLion::Queue<T, Allocator>::Reallocate(ui32 newCapacity)
	{
		T* newLocation = (T*)m_allocator->Allocate(newCapacity * sizeof(T), __alignof(T));

		const ui32 newSize = newCapacity < m_size ? newCapacity : m_size;

		for (ui32 i = 0; i < newSize; ++i)
		{
			newLocation[i] = std::move(m_elements[i]);
		}

		Clear();

		m_elements = newLocation;
		m_capacity = newCapacity;
		m_size = newSize;
	}

	template <typename T, typename Allocator>
	void BaldLion::Queue<T, Allocator>::Reserve(ui32 capacity)
	{
		m_capacity = capacity;
		m_elements = (T*)m_allocator->Allocate(capacity * sizeof(T), __alignof(T));
	}

}
