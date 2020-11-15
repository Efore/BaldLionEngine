#pragma once
#include "BaldLion/Core/Memory/MemoryManager.h"

namespace BaldLion
{
	template <typename T, typename Allocator = Memory::Allocator>
	class DynamicArray
	{
	public:		
		DynamicArray();
		DynamicArray(Memory::AllocationType allocationType, size_t capacity);
		DynamicArray(Memory::AllocationType allocationType, const DynamicArray<T, Allocator>& other);
		DynamicArray(const DynamicArray<T, Allocator>& other);
		DynamicArray(DynamicArray<T, Allocator>&& other) noexcept;

		~DynamicArray();

		size_t Size() const { return m_size; }
		size_t Capacity() const { return m_capacity; }

		void Reserve(size_t capacity);	
		void Free();
		void FreeNoDestructor();

		template <typename... Args >
		void EmplaceBack(Args&&... args);

		void PushBack(const T& element);
		void PushBack(T&& element);

		void PopBack();

		void Remove(const T& element);
		void RemoveFast(const T& element);

		void RemoveAt(size_t index);
		void RemoveAtFast(size_t index);

		void Fill(int size = -1);

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
			BL_ASSERT(m_size > 0 && index < m_size, "Size is 0");
			return index > (m_size - 1) ? Back() : m_elements[index];
		}
		const T& operator[](size_t index) const {  
			BL_ASSERT(m_size > 0 && index < m_size, "Size is 0");
			return index > (m_size - 1) ? Back() : m_elements[index];
		}

		DynamicArray<T,Allocator>& operator= (const DynamicArray<T, Allocator>& other);
		DynamicArray<T,Allocator>& operator= (DynamicArray<T, Allocator>&& other) noexcept;

	private:

		void Reallocate(size_t newCapacity);

	private:

		T* m_elements = nullptr;
		Allocator* m_allocator = nullptr;

		size_t m_size = 0;
		size_t m_capacity = 0;
	};
	
	template <typename T, typename Allocator>
	BaldLion::DynamicArray<T, Allocator>::DynamicArray()
	{
		m_elements = nullptr;
		m_allocator = nullptr;
		m_capacity = 0;
		m_size = 0;
	}

	template <typename T, typename Allocator>
	BaldLion::DynamicArray<T, Allocator>::DynamicArray(Memory::AllocationType allocationType, size_t capacity) 
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

		for (size_t i = 0; i < other.Size(); ++i)
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
	void BaldLion::DynamicArray<T, Allocator>::Reserve(size_t capacity)
	{
		m_capacity = capacity;
		m_elements = (T*)m_allocator->Allocate(capacity * sizeof(T), __alignof(T));
	}

	template <typename T, typename Allocator>
	void BaldLion::DynamicArray<T, Allocator>::Free()
	{
		if (m_elements == nullptr)
			return;

		for (size_t i = 0; i < m_size; ++i)
		{
			m_elements[m_size].~T();
		}

		m_size = 0;
		m_allocator->Deallocate(m_elements);
		m_elements = nullptr;
	}

	template <typename T, typename Allocator>
	void BaldLion::DynamicArray<T, Allocator>::FreeNoDestructor()
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
			Reallocate((size_t)(m_capacity * 1.5f));

		new (&m_elements[m_size]) T(std::forward<Args>(args)...);

		++m_size;
	}

	template <typename T, typename Allocator>
	void BaldLion::DynamicArray<T, Allocator>::PushBack(const T& element)
	{
		BL_ASSERT(m_capacity > 0, "Capacity is 0");

		if (m_size == m_capacity)
			Reallocate((size_t)(m_capacity * 1.5f));

		m_elements[m_size++] = element;
	}

	template <typename T, typename Allocator>
	void BaldLion::DynamicArray<T, Allocator>::PushBack(T&& element)
	{		
		BL_ASSERT(m_capacity > 0, "Capacity is 0");

		if (m_size == m_capacity)
			Reallocate((size_t)(m_capacity * 1.5f));

		m_elements[m_size++] = std::move(element);
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

		size_t i = 0;
		for (; i <= m_size || m_elements[i] != element; ++i);

		RemoveAt(i);
	}

	template <typename T, typename Allocator>
	void BaldLion::DynamicArray<T, Allocator>::RemoveFast(const T& element)
	{
		if (m_size == 0)
			return;

		size_t i = 0;
		for (; i <= m_size || m_elements[i] != element; ++i);

		RemoveAtFast(i);
	}

	template <typename T, typename Allocator>
	void BaldLion::DynamicArray<T, Allocator>::RemoveAt(size_t index)
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
	void BaldLion::DynamicArray<T, Allocator>::RemoveAtFast(size_t index)
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
		for (int i = 0; i < m_size; ++i)
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
	void BaldLion::DynamicArray<T, Allocator>::Reallocate(size_t newCapacity)
	{
		T* newLocation = (T*)m_allocator->Allocate(newCapacity * sizeof(T), __alignof(T));

		const size_t newSize = newCapacity < m_size ? newCapacity : m_size;

		for (size_t i = 0; i < newSize; ++i)
		{
			newLocation[i] = std::move(m_elements[i]);
		}

		Free();

		m_elements = newLocation;
		m_capacity = newCapacity;
		m_size = newSize;
	}
	
	template <typename T, typename Allocator >
	void BaldLion::DynamicArray<T, Allocator>::Fill(int size)
	{
		m_size = size == -1 ? m_capacity : (size_t)size;

		for (size_t i = 0; i < m_size; ++i)
		{
			m_elements[i] = T();
		}
	}
}
