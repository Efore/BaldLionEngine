#pragma once
#include "BaldLion/Core/Memory/MemoryManager.h"

namespace BaldLion
{
	template <typename T>
	class BLVector
	{
	public:		
		BLVector(AllocationType allocationType, size_t capacity);
		~BLVector();

		int Size() const { return m_size; }
		int Capacity() const { return m_capacity; }

		void Reserve(size_t capacity);	
		void Clear();

		template <typename... Args >
		void EmplaceBack(Args&&... args);

		void EmplaceBack();

		void Remove(const T& element);
		void RemoveAt(size_t index);

		bool IsEmpty() const { return m_size == 0; }

		T& operator[](size_t index) { return m_elements[index * sizeof(T)]; }
		const T& operator[](size_t index) const { return m_elements[index * sizeof(T)]; }

	private:

		void Reallocate(size_t newCapacity);

	private:

		T* m_elements = nullptr;
		Allocator* m_allocator = nullptr;

		size_t m_size = 0;
		size_t m_capacity = 0;
	};
	

	template <typename T>
	BaldLion::BLVector<T>::BLVector(AllocationType allocationType, size_t capacity) : m_allocator (MemoryManager::GetAllocator(allocationType))
	{
		Reserve(capacity);
	}

	template <typename T>
	BaldLion::BLVector<T>::~BLVector()
	{

	}

	template <typename T>
	void BaldLion::BLVector<T>::Reserve(size_t capacity)
	{
		m_elements = m_allocator.Allocate(capacity * sizeof(T), __alignof(T));
	}

	template <typename T>
	void BaldLion::BLVector<T>::Clear()
	{

	}

	template <typename T>
	template <typename... Args >
	void BaldLion::BLVector<T>::EmplaceBack(Args&&... args)
	{
		if (m_size == m_capacity)
			Reallocate(m_capacity * 1.5f);

		new (&m_elements[m_size]) T;

		++m_size;
	}

	template <typename T>
	void BaldLion::BLVector<T>::EmplaceBack()
	{
		if (m_size == m_capacity)
			Reallocate(m_capacity * 1.5f);

		new (&m_elements[m_size]) T;

		++m_size;
	}

	template <typename T>
	void BaldLion::BLVector<T>::Remove(const T& element)
	{

	}

	template <typename T>
	void BaldLion::BLVector<T>::RemoveAt(size_t index)
	{

	}

	template <typename T>
	void BaldLion::BLVector<T>::Reallocate(size_t newCapacity)
	{

	}
}
