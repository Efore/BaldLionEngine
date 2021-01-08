#pragma once
#include "BaldLion/Core/Memory/MemoryManager.h"

namespace BaldLion
{
	template <typename T, typename Allocator = Memory::Allocator>
	class DynamicArray
	{
	public:		
		DynamicArray();
		DynamicArray(Memory::AllocationType allocationType, ui32 capacity);
		DynamicArray(Memory::AllocationType allocationType, const DynamicArray<T, Allocator>& other);
		DynamicArray(const DynamicArray<T, Allocator>& other);
		DynamicArray(DynamicArray<T, Allocator>&& other) noexcept;

		~DynamicArray();

		ui32 Size() const { return m_size; }
		ui32 Capacity() const { return m_capacity; }

		void Clear();
		void ClearNoDestructor();

		template <typename... Args >
		void EmplaceBack(Args&&... args);

		void PushBack(const T& element);
		void PushBack(T&& element);

		void PushAt(const T& element, ui32 index);
		void PushAt(T&& element, ui32 index);

		void PopBack();

		void Remove(const T& element);
		void RemoveFast(const T& element);

		void RemoveAt(ui32 index);
		void RemoveAtFast(ui32 index);

		void Fill();		
		void Fill(const T& value);
		void Fill(T&& value);

		int FindIndex(const T& element);
		bool Exists(const T& element);

		bool IsEmpty() const { return m_size == 0; }

		T& Begin() { return m_elements[0]; }
		const T& Begin() const{ return m_elements[0]; }

		//T& End() { return m_elements[0]; }
		//const T& End() const { return m_elements[0]; }

		T& Back() { return m_elements[m_size - 1]; }
		const T& Back() const { return m_elements[m_size - 1]; }

		T& operator[](size_t index) {
			BL_ASSERT(index < m_size, "Index is bigger than size");
			return index > (m_size - 1) ? Back() : m_elements[index];
		}
		const T& operator[](size_t index) const {
			BL_ASSERT(index < m_size, "Index is bigger than size");
			return index > (m_size - 1) ? Back() : m_elements[index];
		}

		DynamicArray<T,Allocator>& operator= (const DynamicArray<T, Allocator>& other);
		DynamicArray<T,Allocator>& operator= (DynamicArray<T, Allocator>&& other) noexcept;

	private:

		void Reserve(ui32 capacity);
		void Reallocate(ui32 newCapacity);

	private:

		T* m_elements = nullptr;
		Allocator* m_allocator = nullptr;

		ui32 m_size = 0;
		ui32 m_capacity = 0;
	};
	
	template <typename T, typename Allocator>
	BaldLion::DynamicArray<T, Allocator>::DynamicArray()
	{	
	} 

	template <typename T, typename Allocator>
	BaldLion::DynamicArray<T, Allocator>::DynamicArray(Memory::AllocationType allocationType, ui32 capacity)
		: m_allocator (Memory::MemoryManager::GetAllocator(allocationType)), m_size(0)
	{
		Reserve(capacity);
	}

	template <typename T, typename Allocator>
	BaldLion::DynamicArray<T, Allocator>::DynamicArray(Memory::AllocationType allocationType, const DynamicArray<T, Allocator>& other)
		: m_allocator(Memory::MemoryManager::GetAllocator(allocationType)), m_size(other.Size())
	{
		Reserve(other.m_capacity);

		std::copy(other.m_elements, other.m_elements + m_size, m_elements);
	}

	template <typename T, typename Allocator>
	BaldLion::DynamicArray<T, Allocator>::DynamicArray(const DynamicArray<T, Allocator>& other)
		: m_allocator(other.m_allocator), m_size(other.Size())
	{
		Reserve(other.m_capacity);
		std::copy(other.m_elements, other.m_elements + m_size, m_elements);
	}

	template <typename T, typename Allocator>
	BaldLion::DynamicArray<T, Allocator>::DynamicArray(DynamicArray<T, Allocator>&& other) noexcept
		: m_allocator(other.m_allocator), m_size(other.Size()), m_capacity(other.m_capacity)
	{
		m_elements = other.m_elements;
		other.m_elements = nullptr;
	}

	template <typename T, typename Allocator>
	DynamicArray<T, Allocator>& BaldLion::DynamicArray<T, Allocator>::operator=(const DynamicArray<T, Allocator>& other)
	{
		if (&other == this)
			return *this;

		m_allocator = other.m_allocator;
		m_size = other.m_size;
		Reserve(other.Capacity());

		for (ui32 i = 0; i < other.Size(); ++i)
		{
			m_elements[i] = other[i];
		}

		return *this;
	}

	template <typename T, typename Allocator>
	DynamicArray<T, Allocator>& BaldLion::DynamicArray<T, Allocator>::operator=(DynamicArray<T, Allocator>&& other) noexcept
	{
		if (&other == this)
			return *this;	

		m_allocator = other.m_allocator;
		m_size = other.m_size;
		m_capacity = other.m_capacity;
		m_elements = other.m_elements;
		other.m_elements = nullptr;

		return *this;
	}	

	template <typename T, typename Allocator>
	BaldLion::DynamicArray<T, Allocator>::~DynamicArray()
	{
		
	}

	template <typename T, typename Allocator>
	void BaldLion::DynamicArray<T, Allocator>::Reserve(ui32 capacity)
	{
		m_capacity = capacity;
		m_elements = (T*)m_allocator->Allocate(capacity * sizeof(T), __alignof(T));
	}

	template <typename T, typename Allocator>
	void BaldLion::DynamicArray<T, Allocator>::Clear()
	{
		if (m_elements == nullptr)
			return;

		for (ui32 i = 0; i < m_size; ++i)
		{
			m_elements[m_size].~T();
		}

		m_size = 0;
		m_allocator->Deallocate(m_elements);
		m_elements = nullptr;
	}

	template <typename T, typename Allocator>
	void BaldLion::DynamicArray<T, Allocator>::ClearNoDestructor()
	{
		if (m_elements == nullptr)
			return;

		m_size = 0;
		m_allocator->Deallocate(m_elements);
		m_elements = nullptr;
	}

	template <typename T, typename Allocator>
	template <typename... Args >
	void BaldLion::DynamicArray<T, Allocator>::EmplaceBack(Args&&... args)
	{
		BL_ASSERT(m_capacity > 0, "Capacity is 0");

		if (m_size == m_capacity)
			Reallocate((ui32)(m_capacity * 1.5f));

		new (&m_elements[m_size]) T(std::forward<Args>(args)...);

		++m_size;
	}

	template <typename T, typename Allocator>
	void BaldLion::DynamicArray<T, Allocator>::PushBack(const T& element)
	{
		BL_ASSERT(m_capacity > 0, "Capacity is 0");

		if (m_size == m_capacity)
			Reallocate((ui32)(m_capacity * 1.5f));

		m_elements[m_size++] = element;
	}

	template <typename T, typename Allocator>
	void BaldLion::DynamicArray<T, Allocator>::PushBack(T&& element)
	{		
		BL_ASSERT(m_capacity > 0, "Capacity is 0");

		if (m_size == m_capacity)
			Reallocate((ui32)(m_capacity * 1.5f));

		m_elements[m_size++] = std::move(element);
	}

	template <typename T, typename Allocator>
	void BaldLion::DynamicArray<T, Allocator>::PushAt(const T& element, ui32 index)
	{
		BL_ASSERT(m_capacity > 0, "Capacity is 0");

		if (index >= m_size - 1)
		{
			PushBack(element);
		}
		else {

			if (m_size == m_capacity)
				Reallocate((ui32)(m_capacity * 1.5f));

			T* newLocation = (T*)m_allocator->Allocate((m_size == m_capacity ? (ui32)(m_capacity * 1.5f) : m_capacity) * sizeof(T), __alignof(T));
			const ui32 newSize = m_size + 1;

			std::copy(m_elements, m_elements + index, newLocation);
			newLocation[index] = element;
			std::copy(m_elements + index + 1, m_elements + m_size, newLocation + index);

			Clear();

			m_elements = newLocation;
			m_size = newSize;
		}
	}

	template <typename T, typename Allocator>
	void BaldLion::DynamicArray<T, Allocator>::PushAt(T&& element, ui32 index)
	{
		BL_ASSERT(m_capacity > 0, "Capacity is 0");
		BL_ASSERT(index < m_size, "Index is bigger than size");

		if (index >= m_size - 1)
		{
			PushBack(element);
		}
		else {

			if (m_size == m_capacity)
				Reallocate((ui32)(m_capacity * 1.5f));

			T* newLocation = (T*)m_allocator->Allocate((m_size == m_capacity ? (ui32)(m_capacity * 1.5f) : m_capacity) * sizeof(T), __alignof(T));
			const ui32 newSize = m_size + 1;

			std::copy(m_elements, m_elements + index, newLocation);
			newLocation[index] = std::move(element);
			std::copy(m_elements + index + 1, m_elements + m_size, newLocation + index);

			Clear();

			m_elements = newLocation;
			m_size = newSize;
		}
	}

	template <typename T, typename Allocator>
	void BaldLion::DynamicArray<T, Allocator>::PopBack()
	{		
		if (m_size == 0)
			return;

		--m_size;
		m_elements[m_size].~T();
	}

	template <typename T, typename Allocator>
	void BaldLion::DynamicArray<T, Allocator>::Remove(const T& element)
	{
		if (m_size == 0)
			return;

		ui32 i = 0;
		for (; i <= m_size && m_elements[i] != element; ++i);

		if (i < m_size)
			RemoveAt(i);
		else
			BL_LOG_CORE_ERROR("Element not found");
	}

	template <typename T, typename Allocator>
	void BaldLion::DynamicArray<T, Allocator>::RemoveFast(const T& element)
	{
		if (m_size == 0)
			return;

		ui32 i = 0;
		for (; i <= m_size && m_elements[i] != element; ++i);

		if (i < m_size)
			RemoveAtFast(i);
		else
			BL_LOG_CORE_ERROR("Element not found");
	}

	template <typename T, typename Allocator>
	void BaldLion::DynamicArray<T, Allocator>::RemoveAt(ui32 index)
	{
		BL_ASSERT(index < m_size, "Index can't be bigger than or equal to m_size");			

		if (index == m_size - 1)
		{
			PopBack();
		}
		else
		{
			T* newLocation = (T*)m_allocator->Allocate(m_capacity * sizeof(T), __alignof(T));
			const ui32 newSize = m_size - 1;

			std::copy(m_elements, m_elements + index, newLocation);
			std::copy(m_elements + index + 1, m_elements + m_size, newLocation + index);

			Clear();

			m_elements = newLocation;
			m_size = newSize;
		}
	}

	template <typename T, typename Allocator>
	void BaldLion::DynamicArray<T, Allocator>::RemoveAtFast(ui32 index)
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
	int BaldLion::DynamicArray<T, Allocator>::FindIndex(const T& element)
	{
		for (ui32 i = 0; i < m_size; ++i)
		{
			if (m_elements[i] == element)
			{
				return i;
			}
		}
		return -1;
	}

	template <typename T, typename Allocator>
	bool BaldLion::DynamicArray<T, Allocator>::Exists(const T& element)
	{
		return FindIndex(element) != -1;
	}

	template <typename T, typename Allocator>
	void BaldLion::DynamicArray<T, Allocator>::Reallocate(ui32 newCapacity)
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
	
	template <typename T, typename Allocator >
	void BaldLion::DynamicArray<T, Allocator>::Fill()
	{
		m_size = m_capacity;

		for (ui32 i = 0; i < m_size; ++i)
		{
			m_elements[i] = T();
		}
	}

	template <typename T, typename Allocator >
	void BaldLion::DynamicArray<T, Allocator>::Fill(const T& value)
	{
		m_size = m_capacity;

		for (ui32 i = 0; i < m_size; ++i)
		{ 
			m_elements[i] = value;
		}
	}

	template <typename T, typename Allocator >
	void BaldLion::DynamicArray<T, Allocator>::Fill(T&& value)
	{
		m_size = m_capacity;

		for (ui32 i = 0; i < m_size; ++i)
		{
			m_elements[i] = value;
		}
	}
}
