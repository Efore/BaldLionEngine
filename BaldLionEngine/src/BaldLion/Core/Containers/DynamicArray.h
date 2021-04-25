#pragma once
#include "BaldLion/Core/Memory/MemoryManager.h"

namespace BaldLion
{
	template <typename T>
	class DynamicArray
	{
	public:		
		DynamicArray() : m_elements(nullptr) {};
		DynamicArray(Memory::AllocationType allocationType, ui32 capacity);
		DynamicArray(Memory::AllocationType allocationType, const DynamicArray<T>& other);
		DynamicArray(const DynamicArray<T>& other);
		DynamicArray(DynamicArray<T>&& other) noexcept;

		~DynamicArray();

		ui32 Size() const { return m_size; }
		ui32 Capacity() const { return m_capacity; }

		void Clear();
		void ClearNoDestructor();

		template <typename... Args >
		void EmplaceBack(Args&&... args);

		void PushBackRange(const DynamicArray<T>& other, ui32 size = 0);
		void PushBack(const T& element);
		void PushBack(T&& element);

		void PushAt(const T& element, ui32 index);
		void PushAt(T&& element, ui32 index);

		void PopBack();

		void Remove(const T& element);
		void RemoveFast(const T& element);

		void RemoveAt(ui32 index);
		void RemoveAtFast(ui32 index);

		void Populate();		
		void Populate(const T& value);
		void Populate(T&& value);

		int FindIndex(const T& element);
		bool Contains(const T& element);

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

		DynamicArray<T>& operator= (const DynamicArray<T>& other);
		DynamicArray<T>& operator= (DynamicArray<T>&& other) noexcept;

	private:

		void Reserve(ui32 capacity);
		void Reallocate(ui32 newCapacity);

	private:

		T* m_elements = nullptr;
		AllocationType m_allocationType;

		ui32 m_size = 0;
		ui32 m_capacity = 0;
	};	


	template <typename T>
	BaldLion::DynamicArray<T>::DynamicArray(Memory::AllocationType allocationType, ui32 capacity)
		: m_allocationType (allocationType), m_size(0)
	{
		Reserve(capacity);
	}

	template <typename T>
	BaldLion::DynamicArray<T>::DynamicArray(Memory::AllocationType allocationType, const DynamicArray<T>& other)
		: m_allocationType(allocationType), m_size(other.Size())
	{
		Reserve(other.m_capacity);
		std::copy(other.m_elements, other.m_elements + m_size, m_elements);
	}

	template <typename T>
	BaldLion::DynamicArray<T>::DynamicArray(const DynamicArray<T>& other)
		: m_allocationType(other.m_allocationType), m_size(other.Size())
	{
		Reserve(other.m_capacity);
		std::copy(other.m_elements, other.m_elements + m_size, m_elements);
	}

	template <typename T>
	BaldLion::DynamicArray<T>::DynamicArray(DynamicArray<T>&& other) noexcept
		: m_allocationType(other.m_allocationType), m_size(other.Size()), m_capacity(other.m_capacity)
	{
		m_elements = other.m_elements;

		other.m_elements = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
	}

	template <typename T>
	DynamicArray<T>& BaldLion::DynamicArray<T>::operator=(const DynamicArray<T>& other)
	{
		if (&other == this || other.m_elements == nullptr)			
			return *this;

		//Clear();
		
		m_allocationType = other.m_allocationType;
		m_size = other.m_size;

		Reserve(other.Capacity());

		std::copy(other.m_elements, other.m_elements + m_size, m_elements);
		
		return *this;
	}

	template <typename T>
	DynamicArray<T>& BaldLion::DynamicArray<T>::operator=(DynamicArray<T>&& other) noexcept
	{
		if (&other == this || other.m_elements == nullptr)
			return *this;	

		//Clear();

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
	BaldLion::DynamicArray<T>::~DynamicArray()
	{

	}

	template <typename T>
	void BaldLion::DynamicArray<T>::Clear()
	{
		if (m_elements == nullptr)
			return;

		for (ui32 i = 0; i < m_size; ++i)
		{
			m_elements[m_size].~T();
		}

		ClearNoDestructor();
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::ClearNoDestructor()
	{
		if (m_elements == nullptr)
			return;

		m_size = 0;
		m_capacity = 0;
		MemoryManager::DeleteNoDestructor(m_elements, true);
		m_elements = nullptr;
	}

	template <typename T>
	template <typename... Args >
	void BaldLion::DynamicArray<T>::EmplaceBack(Args&&... args)
	{
		BL_DEEP_PROFILE_FUNCTION();

		BL_ASSERT(m_capacity > 0, "Capacity is 0");

		if (m_size == m_capacity)
		{	
			Reallocate((ui32)(m_capacity * 1.5f));		
		}

		new (&m_elements[m_size++]) T(std::forward<Args>(args)...);
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::PushBack(const T& element)
	{
		BL_DEEP_PROFILE_FUNCTION();

		BL_ASSERT(m_capacity > 0, "Capacity is 0");

		if (m_size == m_capacity)
		{	
			Reallocate((ui32)(m_capacity * 1.5f));		
		}

		m_elements[m_size++] = element;		
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::PushBack(T&& element)
	{	
		BL_ASSERT(m_capacity > 0, "Capacity is 0");

		if (m_size == m_capacity)
		{		
			Reallocate((ui32)(m_capacity * 1.5f));		
		}

		m_elements[m_size++] = std::move(element);		
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::PushBackRange(const DynamicArray<T>& other, ui32 size)
	{
		if (size == 0) size = other.m_size;

		if (m_size + size > m_capacity)
			Reallocate(m_capacity + size);

		memcpy(m_elements + sizeof(T) * m_size, other.m_elements, size * sizeof(T));
		m_size += size;
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::PushAt(const T& element, ui32 index)
	{
		BL_ASSERT(m_capacity > 0, "Capacity is 0");

		if (index >= m_size - 1)
		{	
			PushBack(element);
		}
		else {

			if (m_size == m_capacity)
			{
				Reallocate((ui32)(m_capacity * 1.5f));
			}

			T* newLocation = MemoryManager::NewArray<T>("Dynamic Array", m_allocationType, m_size == m_capacity ? (ui32)(m_capacity * 1.5f) : m_capacity);
			const ui32 newSize = m_size + 1;

			std::copy(m_elements, m_elements + index, newLocation);
			newLocation[index] = element;
			std::copy(m_elements + index + 1, m_elements + m_size, newLocation + index);
			
			Clear();

			m_elements = newLocation;
			m_size = newSize;
		}
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::PushAt(T&& element, ui32 index)
	{
		BL_ASSERT(m_capacity > 0, "Capacity is 0");
		BL_ASSERT(index < m_size, "Index is bigger than size");

		if (index >= m_size - 1)
		{			
			PushBack(element);
		}
		else {

			if (m_size == m_capacity)
			{				
				Reallocate((ui32)(m_capacity * 1.5f));				
			}

			T* newLocation = MemoryManager::NewArray<T>(STRING_TO_ID("Dynamic Array"), m_allocationType, m_size == m_capacity ? (ui32)(m_capacity * 1.5f) : m_capacity);
			const ui32 newSize = m_size + 1;

			std::copy(m_elements, m_elements + index, newLocation);
			newLocation[index] = std::move(element);
			std::copy(m_elements + index + 1, m_elements + m_size, newLocation + index);

			Clear();

			m_elements = newLocation;
			m_size = newSize;			
		}
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::PopBack()
	{		
		if (m_size == 0)
			return;

		--m_size;
		m_elements[m_size].~T();
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::Remove(const T& element)
	{
		if (m_size == 0)
		{
			return;
		}

		ui32 i = 0;
		for (; i <= m_size && m_elements[i] != element; ++i);

		if (i < m_size)
		{
			RemoveAt(i);
		}
		else {
			BL_LOG_CORE_ERROR("Element not found");
		}
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::RemoveFast(const T& element)
	{
		if (m_size == 0)
		{
			return;
		}

		ui32 i = 0;
		for (; i <= m_size && m_elements[i] != element; ++i);

		if (i < m_size)
		{			
			RemoveAtFast(i);
		}
		else
		{
			BL_LOG_CORE_ERROR("Element not found");			
		}
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::RemoveAt(ui32 index)
	{
		BL_ASSERT(index < m_size, "Index can't be bigger than or equal to m_size");			

		if (index == m_size - 1)
		{			
			PopBack();
		}
		else
		{
			T* newLocation = MemoryManager::NewArray<T>(STRING_TO_ID("Dynamic Array"), m_allocationType, m_capacity);
			const ui32 newSize = m_size - 1;

			std::copy(m_elements, m_elements + index, newLocation);
			std::copy(m_elements + index + 1, m_elements + m_size, newLocation + index);

			Clear();

			m_elements = newLocation;
			m_size = newSize;
		}
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::RemoveAtFast(ui32 index)
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

	template <typename T>
	int BaldLion::DynamicArray<T>::FindIndex(const T& element)
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

	template <typename T>
	bool BaldLion::DynamicArray<T>::Contains(const T& element)
	{
		return FindIndex(element) != -1;
	}
	   	
	template <typename T>
	void BaldLion::DynamicArray<T>::Populate()
	{
		m_size = m_capacity;

		for (ui32 i = 0; i < m_size; ++i)
		{
			m_elements[i] = T();
		}
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::Populate(const T& value)
	{	
		m_size = m_capacity;

		for (ui32 i = 0; i < m_size; ++i)
		{ 
			m_elements[i] = value;
		}
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::Populate(T&& value)
	{
		m_size = m_capacity;

		for (ui32 i = 0; i < m_size; ++i)
		{
			m_elements[i] = std::move(value);
		}
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::Reserve(ui32 capacity)
	{
		m_capacity = capacity;
		m_elements = MemoryManager::NewArray<T>("Dynamic array", m_allocationType, capacity);
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::Reallocate(ui32 newCapacity)
	{	
		BL_DEEP_PROFILE_FUNCTION();
		T* newLocation = MemoryManager::NewArray<T>("Dynamic array", m_allocationType, newCapacity);

		const ui32 newSize = newCapacity < m_size ? newCapacity : m_size;

		{
			BL_DEEP_PROFILE_SCOPE("Moving", Optick::Category::None);

			memcpy(newLocation, m_elements, sizeof(T) * newSize);

			//for (ui32 i = 0; i < newSize; ++i)
			//{
			//	newLocation[i] = std::move(m_elements[i]);
			//}
		}
		
		{
			BL_DEEP_PROFILE_SCOPE("Clearing old data", Optick::Category::None);
			Clear();
		}

		m_elements = newLocation;
		m_capacity = newCapacity;
		m_size = newSize;
	}
}
