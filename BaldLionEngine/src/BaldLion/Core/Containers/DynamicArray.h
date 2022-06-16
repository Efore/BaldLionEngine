#pragma once
#include "BaldLion/Core/Memory/MemoryManager.h"

namespace BaldLion
{
	using namespace Memory;

	template <typename T>
	class DynamicArray
	{
	public:		
		DynamicArray() = default;

		DynamicArray(Memory::AllocationType allocationType, ui32 capacity);
		DynamicArray(Memory::AllocationType allocationType, const DynamicArray<T>& other);
		DynamicArray(const DynamicArray<T>& other);
		DynamicArray(DynamicArray<T>&& other) noexcept;

		~DynamicArray();

		ui32 Size() const { return m_size; }
		ui32 Capacity() const { return m_capacity; }

		void Clear();
		void Delete();
		void DeleteNoDestructor();

		template <typename... Args >
		T* EmplaceBack(Args&&... args);

		void PushBackRange(const DynamicArray<T>& other, ui32 size = 0);
		void PushBackRange(const T* other, ui32 size = 0);

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
		ui32 m_size = 0;
		ui32 m_capacity = 0;

		AllocationType m_allocationType;
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
		if (m_capacity > 0)
		{
			memcpy(m_elements, other.m_elements, m_size * sizeof(T));
		}
	}

	template <typename T>
	BaldLion::DynamicArray<T>::DynamicArray(const DynamicArray<T>& other)
		: m_allocationType(other.m_allocationType), m_size(other.Size())
	{
		Reserve(other.m_capacity);
		if (m_capacity > 0)
		{
			memcpy(m_elements, other.m_elements, m_size * sizeof(T));
		}
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
		BL_DEEP_PROFILE_FUNCTION();

		if (&other == this)			
			return *this;

		Delete();
		
		m_allocationType = other.m_allocationType;
		m_size = other.m_size;

		Reserve(other.Capacity());

		if (other.m_elements != nullptr)
		{
			memcpy(m_elements, other.m_elements, m_size * sizeof(T));			
		}
		else 
		{
			m_elements = nullptr;
		}

		
		return *this;
	}

	template <typename T>
	DynamicArray<T>& BaldLion::DynamicArray<T>::operator=(DynamicArray<T>&& other) noexcept
	{
		BL_DEEP_PROFILE_FUNCTION();

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
	BaldLion::DynamicArray<T>::~DynamicArray()
	{
		//if (m_elements != nullptr)
		//{
		//	DeleteNoDestructor();
		//}
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::Clear()
	{
		if (m_elements == nullptr)
			return;

		m_size = 0;
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::Delete()
	{
		BL_DEEP_PROFILE_FUNCTION();

		if (m_elements == nullptr)
			return;

		for (ui32 i = 0; i < m_size; ++i)
		{
			m_elements[i].~T();
		}

		DeleteNoDestructor();
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::DeleteNoDestructor()
	{
		if (m_elements == nullptr)
			return;

		m_size = 0;
		m_capacity = 0;
		MemoryManager::DeleteNoDestructor(m_elements);
		m_elements = nullptr;
	}

	template <typename T>
	template <typename... Args >
	T* BaldLion::DynamicArray<T>::EmplaceBack(Args&&... args)
	{
		BL_DEEP_PROFILE_FUNCTION();

		BL_ASSERT(m_capacity > 0, "Capacity is 0");

		if (m_size == m_capacity)
		{	
			Reallocate((ui32)(m_capacity * 1.5f));		
		}

		return new (&m_elements[m_size++]) T(std::forward<Args>(args)...);
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
		BL_DEEP_PROFILE_FUNCTION();

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
		BL_DEEP_PROFILE_FUNCTION();

		if (size == 0) size = other.m_size;

		if ((m_size + size) > m_capacity)
			Reallocate(m_size + size);

		memcpy(&(m_elements[m_size]), other.m_elements, size * sizeof(T));
		m_size += size;
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::PushBackRange(const T* other, ui32 size)
	{
		BL_DEEP_PROFILE_FUNCTION();

		if (m_size + size > m_capacity)
			Reallocate(m_size + size);

		memcpy(&(m_elements[m_size]), other, size * sizeof(T));
		m_size += size;
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::PushAt(const T& element, ui32 index)
	{
		BL_DEEP_PROFILE_FUNCTION();

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

			if (index > 0)
			{
				memcpy(newLocation, m_elements, index * sizeof(T));
			}

			newLocation[index] = element;

			memcpy(&(newLocation[index+1]), &m_elements[index], (m_size - index) * sizeof(T));

			Delete();

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

			T* newLocation = MemoryManager::NewArray<T>("Dynamic Array", m_allocationType, m_size == m_capacity ? (ui32)(m_capacity * 1.5f) : m_capacity);
			const ui32 newSize = m_size + 1;

			if (index > 0)
			{
				memcpy(newLocation, m_elements, index * sizeof(T));
			}

			newLocation[index] = std::move(element);

			memcpy(&(newLocation[index + 1]), &m_elements[index], (m_size - index) * sizeof(T));

			Delete();

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
			const ui32 capacity = m_capacity;
			T* newLocation = MemoryManager::NewArray<T>("Dynamic Array", m_allocationType, capacity);
			const ui32 newSize = m_size - 1;

			if (index > 0)
			{
				memcpy(newLocation, m_elements, index * sizeof(T));
			}

			memcpy(&(newLocation[index]), &(m_elements[index+1]), (m_size - (index + 1)) * sizeof(T));

			Delete();

			m_elements = newLocation;
			m_size = newSize;
			m_capacity = capacity;
		}
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::RemoveAtFast(ui32 index)
	{		
		BL_ASSERT(index < m_size, "Index can't be bigger than or equal to m_size");

		if (index == (m_size - 1))
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
			T defaultValue;
			memcpy(&(m_elements[i]), (T*)(&defaultValue), sizeof(T));
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
		if (capacity > 0)
		{
			m_elements = MemoryManager::NewArray<T>("Dynamic array", m_allocationType, capacity);
		}
	}

	template <typename T>
	void BaldLion::DynamicArray<T>::Reallocate(ui32 newCapacity)
	{	
		BL_DEEP_PROFILE_FUNCTION();

		if (newCapacity == 0)
		{
			m_elements = nullptr;
			m_capacity = 0;
			m_size = 0;

			return;
		}

		T* newLocation = MemoryManager::NewArray<T>("Dynamic array", m_allocationType, newCapacity);

		const ui32 newSize = newCapacity < m_size ? newCapacity : m_size;

		memcpy(newLocation, m_elements, sizeof(T) * newSize);

		Delete();		

		m_elements = newLocation;
		m_capacity = newCapacity;
		m_size = newSize;
	}

	#define BL_DYNAMICARRAY_FOREACH(dynamicArrayToIterate) \
		for(ui32 i = 0, size = (dynamicArrayToIterate).Size(); i < size; ++i)


	#define BL_DYNAMICARRAY_FOR(iteratorName, dynamicArrayToIterate, startValue) \
		for(ui32 iteratorName = startValue, size = (dynamicArrayToIterate).Size(); iteratorName < size; ++iteratorName)

	#define BL_DYNAMICARRAY_INVERSE_FOR(iteratorName, dynamicArrayToIterate, endValue) \
		for(ui32 iteratorName = (dynamicArrayToIterate).Size() - 1; iteratorName >= endValue; --iteratorName)

	#define BL_DYNAMICARRAY_FOR_RANGE(iteratorName, dynamicArrayToIterate, startValue, endValue) \
		for(ui32 iteratorName = startValue, size = (dynamicArrayToIterate).Size(); iteratorName < size && iteratorName < endValue; ++iteratorName)
}
