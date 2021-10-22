#pragma once
#include "BaldLion/Core/Memory/MemoryManager.h"

namespace BaldLion
{
	template <typename T>
	class Queue
	{
	public:
		Queue();
		Queue(Memory::AllocationType allocationType, ui32 capacity);
		Queue(Memory::AllocationType allocationType, const Queue<T>& other);
		Queue(const Queue<T>& other);
		Queue(Queue<T>&& other) noexcept;

		~Queue();

		ui32 Size() const { return m_size - m_frontIndex; }
		ui32 Capacity() const { return m_capacity; }

		void Clear();
		void Delete();
		void DeleteNoDestructor();

		template <typename... Args >
		void EmplaceBack(Args&&... args);

		void PushBack(const T& element);
		void PushBack(T&& element);

		T& Front();
		const T& Front() const;

		void Pop();

		bool Exists(const T& element);

		bool IsEmpty() const { return m_size - m_frontIndex == 0; }

		Queue<T>& operator= (const Queue<T>& other);
		Queue<T>& operator= (Queue<T>&& other) noexcept;

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

	template <typename T>
	BaldLion::Queue<T>::Queue()
	{
	}

	template <typename T>
	BaldLion::Queue<T>::Queue(Memory::AllocationType allocationType, ui32 capacity)
		: m_allocationType(allocationType), m_size(0)
	{
		Reserve(capacity);
	}

	template <typename T>
	BaldLion::Queue<T>::Queue(Memory::AllocationType allocationType, const Queue<T>& other)
		: m_allocationType(allocationType), m_size(other.Size()), m_frontIndex(other.m_frontIndex)
	{
		Reserve(other.m_capacity);
		memcpy(m_elements, other.m_elements, m_size * sizeof(T));
	}

	template <typename T>
	BaldLion::Queue<T>::Queue(const Queue<T>& other)
		: m_allocationType(other.m_allocationType), m_size(other.Size()), m_frontIndex(other.m_frontIndex)
	{
		Reserve(other.m_capacity);
		memcpy(m_elements, other.m_elements, m_size * sizeof(T));
	}

	template <typename T>
	BaldLion::Queue<T>::Queue(Queue<T>&& other) noexcept
		: m_allocationType(other.m_allocationType), m_size(other.Size()), m_capacity(other.m_capacity), m_frontIndex(other.m_frontIndex)
	{
		m_elements = other.m_elements;
		
		other.m_elements = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
	}

	template <typename T>
	Queue<T>& BaldLion::Queue<T>::operator=(const Queue<T>& other)
	{		
		if (&other == this)
			return *this;

		m_allocationType = other.m_allocationType;
		m_size = other.m_size;
		m_frontIndex = other.m_frontIndex;

		Reserve(other.Capacity());

		memcpy(m_elements, other.m_elements, m_size * sizeof(T));

		return *this;
	}

	template <typename T>
	Queue<T>& BaldLion::Queue<T>::operator=(Queue<T>&& other) noexcept
	{
		if (&other == this)
			return *this;

		Delete();

		m_allocationType = other.m_allocationType;
		m_size = other.m_size;
		m_capacity = other.m_capacity;
		m_elements = other.m_elements;

		other.m_elements = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
		
		return *this;
	}

	template <typename T>
	BaldLion::Queue<T>::~Queue()
	{
		
	}

	template <typename T>
	void BaldLion::Queue<T>::Clear()
	{
		if (m_elements == nullptr)
		{
			return;
		}

		for (ui32 i = 0; i < m_size; ++i)
		{
			m_elements[i].~T();
		}

		m_size = 0;
		m_frontIndex = 0;
	}

	template <typename T>
	void BaldLion::Queue<T>::Delete()
	{
		if (m_elements == nullptr)
		{
			return;
		}

		for (ui32 i = 0; i < m_size; ++i)
		{
			m_elements[i].~T();
		}

		DeleteNoDestructor();
	}

	template <typename T>
	void BaldLion::Queue<T>::DeleteNoDestructor()
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

	template <typename T>
	template <typename... Args >
	void BaldLion::Queue<T>::EmplaceBack(Args&&... args)
	{
		BL_ASSERT(m_capacity > 0, "Capacity is 0");

		if (m_size == m_capacity)
		{
			Reallocate((ui32)(m_capacity * 1.5f));
		}

		new (&m_elements[m_size++]) T(std::forward<Args>(args)...);
	}

	template <typename T>
	void BaldLion::Queue<T>::PushBack(const T& element)
	{
		BL_ASSERT(m_capacity > 0, "Capacity is 0");

		if (m_size == m_capacity)
		{
			Reallocate((ui32)(m_capacity * 1.5f));
		}

		m_elements[m_size++] = element;
	}

	template <typename T>
	void BaldLion::Queue<T>::PushBack(T&& element)
	{
		BL_ASSERT(m_capacity > 0, "Capacity is 0");

		if (m_size == m_capacity)
		{
			Reallocate((ui32)(m_capacity * 1.5f));
		}

		m_elements[m_size++] = std::move(element);
	}

	template <typename T>
	bool BaldLion::Queue<T>::Exists(const T& element)
	{
		return FindIndex(element) != -1;
	}
	
	template <typename T>
	T& BaldLion::Queue<T>::Front()
	{
		BL_ASSERT(m_size > 0, "Size is 0");		
		return m_elements[m_frontIndex];
	}

	template <typename T>
	const T& BaldLion::Queue<T>::Front() const
	{
		BL_ASSERT(m_size > 0, "Size is 0");
		return m_elements[m_frontIndex];
	}

	template <typename T>
	void BaldLion::Queue<T>::Pop()
	{
		BL_ASSERT(m_size > 0, "Size is 0");

		if (++m_frontIndex == m_size)
		{
			m_frontIndex = 0;
			m_size = 0;
		}
	}

	template <typename T>
	void BaldLion::Queue<T>::Reallocate(ui32 newCapacity)
	{
		T* newLocation = MemoryManager::NewArray<T>("Queue", m_allocationType, newCapacity);

		const ui32 newSize = newCapacity < m_size ? newCapacity : m_size;

		for (ui32 i = 0; i < newSize; ++i)
		{
			newLocation[i] = std::move(m_elements[i]);
		}

		Delete();

		m_elements = newLocation;
		m_capacity = newCapacity;
		m_size = newSize;
	}

	template <typename T>
	void BaldLion::Queue<T>::Reserve(ui32 capacity)
	{
		m_capacity = capacity;
		m_elements = MemoryManager::NewArray<T>("Queue", m_allocationType, capacity);
	}

}
