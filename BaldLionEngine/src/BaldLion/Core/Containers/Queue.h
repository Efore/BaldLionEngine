#pragma once
#include "BaldLion/Core/Memory/MemoryManager.h"

namespace BaldLion
{
	template <typename T, typename AllocationType = Memory::AllocationType>
	class Queue
	{
	public:
		Queue();
		Queue(Memory::AllocationType allocationType, ui32 capacity);
		Queue(Memory::AllocationType allocationType, const Queue<T, AllocationType>& other);
		Queue(const Queue<T, AllocationType>& other);
		Queue(Queue<T, AllocationType>&& other) noexcept;

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

		Queue<T, AllocationType>& operator= (const Queue<T, AllocationType>& other);
		Queue<T, AllocationType>& operator= (Queue<T, AllocationType>&& other) noexcept;

	private:

		void Reserve(ui32 capacity);
		void Reallocate(ui32 newCapacity);

	private:

		T* m_elements = nullptr;
		AllocationType m_allocationType;

		ui32 m_frontIndex = 0;
		ui32 m_size = 0;
		ui32 m_capacity = 0;
	};

	template <typename T, typename AllocationType>
	BaldLion::Queue<T, AllocationType>::Queue()
	{
	}

	template <typename T, typename AllocationType>
	BaldLion::Queue<T, AllocationType>::Queue(Memory::AllocationType allocationType, ui32 capacity)
		: m_allocationType(allocationType), m_size(0)
	{
		Reserve(capacity);
	}

	template <typename T, typename AllocationType>
	BaldLion::Queue<T, AllocationType>::Queue(Memory::AllocationType allocationType, const Queue<T, AllocationType>& other)
		: m_allocationType(allocationType), m_size(other.Size()), m_frontIndex(other.m_frontIndex)
	{
		Reserve(other.m_capacity);

		std::copy(other.m_elements, other.m_elements + m_size, m_elements);
	}

	template <typename T, typename AllocationType>
	BaldLion::Queue<T, AllocationType>::Queue(const Queue<T, AllocationType>& other)
		: m_allocationType(other.m_allocationType), m_size(other.Size()), m_frontIndex(other.m_frontIndex)
	{
		Reserve(other.m_capacity);
		std::copy(other.m_elements, other.m_elements + m_size, m_elements);
	}

	template <typename T, typename AllocationType>
	BaldLion::Queue<T, AllocationType>::Queue(Queue<T, AllocationType>&& other) noexcept
		: m_allocationType(other.m_allocationType), m_size(other.Size()), m_capacity(other.m_capacity), m_frontIndex(other.m_frontIndex)
	{
		m_elements = other.m_elements;
		
		other.m_elements = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
	}

	template <typename T, typename AllocationType>
	Queue<T, AllocationType>& BaldLion::Queue<T, AllocationType>::operator=(const Queue<T, AllocationType>& other)
	{		
		if (&other == this)
			return *this;

		m_allocationType = other.m_allocationType;
		m_size = other.m_size;
		m_frontIndex = other.m_frontIndex;
		Reserve(other.Capacity());

		std::copy(other.m_elements, other.m_elements + m_size, m_elements);

		return *this;
	}

	template <typename T, typename AllocationType>
	Queue<T, AllocationType>& BaldLion::Queue<T, AllocationType>::operator=(Queue<T, AllocationType>&& other) noexcept
	{
		if (&other == this)
			return *this;

		Clear();

		m_allocationType = other.m_allocationType;
		m_size = other.m_size;
		m_capacity = other.m_capacity;
		m_elements = other.m_elements;

		other.m_elements = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
		
		return *this;
	}

	template <typename T, typename AllocationType>
	BaldLion::Queue<T, AllocationType>::~Queue()
	{
		
	}
	
	template <typename T, typename AllocationType>
	void BaldLion::Queue<T, AllocationType>::Clear()
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

	template <typename T, typename AllocationType>
	void BaldLion::Queue<T, AllocationType>::ClearNoDestructor()
	{
		if (m_elements == nullptr)
		{
			return;
		}

		m_size = 0;
		m_frontIndex = 0;
		MemoryManager::DeleteNoDestructor(m_elements);
		m_elements = nullptr;
	}

	template <typename T, typename AllocationType>
	template <typename... Args >
	void BaldLion::Queue<T, AllocationType>::EmplaceBack(Args&&... args)
	{
		BL_ASSERT(m_capacity > 0, "Capacity is 0");

		if (m_size == m_capacity)
		{
			Reallocate((ui32)(m_capacity * 1.5f));
		}

		new (&m_elements[m_size++]) T(std::forward<Args>(args)...);
	}

	template <typename T, typename AllocationType>
	void BaldLion::Queue<T, AllocationType>::PushBack(const T& element)
	{
		BL_ASSERT(m_capacity > 0, "Capacity is 0");

		if (m_size == m_capacity)
		{
			Reallocate((ui32)(m_capacity * 1.5f));
		}

		m_elements[m_size++] = element;
	}

	template <typename T, typename AllocationType>
	void BaldLion::Queue<T, AllocationType>::PushBack(T&& element)
	{
		BL_ASSERT(m_capacity > 0, "Capacity is 0");

		if (m_size == m_capacity)
		{
			Reallocate((ui32)(m_capacity * 1.5f));
		}

		m_elements[m_size++] = std::move(element);
	}

	template <typename T, typename AllocationType>
	bool BaldLion::Queue<T, AllocationType>::Exists(const T& element)
	{
		return FindIndex(element) != -1;
	}
	
	template <typename T, typename AllocationType >
	T& BaldLion::Queue<T, AllocationType>::Front()
	{
		BL_ASSERT(m_size > 0, "Size is 0");		
		return m_elements[m_frontIndex];
	}

	template <typename T, typename AllocationType >
	const T& BaldLion::Queue<T, AllocationType>::Front() const
	{
		BL_ASSERT(m_size > 0, "Size is 0");
		return m_elements[m_frontIndex];
	}

	template <typename T, typename AllocationType>
	void BaldLion::Queue<T, AllocationType>::Pop()
	{
		BL_ASSERT(m_size > 0, "Size is 0");

		if (++m_frontIndex == m_size)
		{
			m_frontIndex = 0;
			m_size = 0;
		}
	}

	template <typename T, typename AllocationType>
	void BaldLion::Queue<T, AllocationType>::Reallocate(ui32 newCapacity)
	{
		T* newLocation = MemoryManager::NewArray<T>(STRING_TO_ID("Queue"), m_allocationType, newCapacity);

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

	template <typename T, typename AllocationType>
	void BaldLion::Queue<T, AllocationType>::Reserve(ui32 capacity)
	{
		m_capacity = capacity;
		m_elements = MemoryManager::NewArray<T>(STRING_TO_ID("Queue"), m_allocationType, capacity);
	}

}
