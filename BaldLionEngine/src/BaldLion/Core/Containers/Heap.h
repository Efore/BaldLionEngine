#pragma once
#include "BaldLion/Core/Containers/DynamicArray.h"
#include <functional>

namespace BaldLion
{
	template <typename T, typename Comparison = std::less<>>
	class Heap
	{
	public:
		Heap() = default;
		Heap(Memory::AllocationType allocationType, ui32 capacity);
		Heap(Memory::AllocationType allocationType, const Heap<T>& other);
		Heap(const Heap<T>& other);
		Heap(Heap<T>&& other);

		~Heap();

		ui32 Size() const { return m_content.Size(); }
		ui32 Capacity() const { return m_content.Capacity(); }

		void Clear();
		void Delete();
		void DeleteNoDestructor();

		i32 FindIndex(const T& value);
		const T* GetAtIndex(ui32 index) const;

		void Emplace(const T& value);

		T Pop();
		const T& Top() const;

		void Sort();
		void Print();


	private:

		bool Compare(const T& a, const T& b, Comparison comp = Comparison{});
		void Swap(ui32* a, ui32* b);
		void Heapify(ui32 index, ui32 size);

	private:

		DynamicArray<T> m_content;
		DynamicArray<ui32> m_indexHeap;

		AllocationType m_allocationType = AllocationType::FreeList_Main;

		ui32 m_size = 0;

	};


	template <typename T, typename Comparison /*= std::less<>*/>
	const T* BaldLion::Heap<T, Comparison>::GetAtIndex(ui32 index) const
	{
		BL_ASSERT_LOG(index < m_size, "Index is bigger than size");

		return &m_content[index];
	}

	template <typename T, typename Comparison>
	bool BaldLion::Heap<T, Comparison>::Compare(const T& a, const T& b, Comparison comp)
	{
		return comp(a, b);
	}

	template <typename T, typename Comparison>
	i32 BaldLion::Heap<T, Comparison>::FindIndex(const T& value)
	{
		BL_DYNAMICARRAY_FOREACH(m_content)
		{
			if (m_content[i] == value)
			{
				return i;
			}
		}
		return -1;
	}

	template <typename T, typename Comparison>
	BaldLion::Heap<T, Comparison>::Heap(Memory::AllocationType allocationType, ui32 capacity)
		: m_allocationType(allocationType), m_size(0)
	{
		m_content = DynamicArray<T>(allocationType, capacity);
		m_indexHeap = DynamicArray<ui32>(allocationType, capacity);		
	}

	template <typename T, typename Comparison>
	BaldLion::Heap<T, Comparison>::Heap(Memory::AllocationType allocationType, const Heap<T>& other)
		: m_allocationType(allocationType), m_size(other.m_size)
	{
		m_content = DynamicArray<T>(allocationType, other.m_content);
		m_indexHeap = DynamicArray<ui32>(allocationType, other.m_indexHeap);
	}

	template <typename T, typename Comparison>
	BaldLion::Heap<T, Comparison>::Heap(const Heap<T>& other)
		: m_allocationType(other.m_allocationType), m_size(other.m_size)
	{
		m_content = DynamicArray<T>(other.m_content);
		m_indexHeap = DynamicArray<ui32>(other.m_indexHeap);
	}
	
	template <typename T, typename Comparison>
	BaldLion::Heap<T, Comparison>::Heap(Heap<T>&& other)
		: m_allocationType(other.m_allocationType), m_size(other.m_size)
	{
		m_content = DynamicArray<T>(other.m_content);
		m_indexHeap = DynamicArray<ui32>(other.m_indexHeap);
	}

	template <typename T, typename Comparison>
	BaldLion::Heap<T, Comparison>::~Heap()
	{

	}

	template <typename T, typename Comparison>
	void BaldLion::Heap<T, Comparison>::DeleteNoDestructor()
	{
		m_size = 0;
		m_content.DeleteNoDestructor();
		m_indexHeap.DeleteNoDestructor();
	}

	template <typename T, typename Comparison>
	void BaldLion::Heap<T, Comparison>::Delete()
	{
		m_size = 0;
		m_content.Delete();
		m_indexHeap.Delete();
	}

	template <typename T, typename Comparison>
	void BaldLion::Heap<T, Comparison>::Clear()
	{
		m_size = 0;
		m_content.Clear();
		m_indexHeap.Clear();
	}

	template <typename T, typename Comparison>
	void BaldLion::Heap<T, Comparison>::Sort()
	{
		if (m_size > 1)
		{
			for (i32 i = (m_size / 2) - 1; i >= 0; --i)
			{
				Heapify(i, m_size);
			}

			BL_DYNAMICARRAY_INVERSE_FOREACH(m_indexHeap)
			{
				Swap(&m_indexHeap[0], &m_indexHeap[i]);
				Heapify(0, i);
			}
		}
	}

	template <typename T, typename Comparison>
	void BaldLion::Heap<T, Comparison>::Heapify(ui32 index, ui32 size)
	{
		ui32 largestChild = index;

		ui32 leftChild = 2 * index + 1;
		ui32 rightChild = 2 * index + 2;

		ui32 largestIndex = m_indexHeap[largestChild];

		if (leftChild < size && Compare(m_content[largestIndex], m_content[m_indexHeap[leftChild]]))
		{
			largestChild = leftChild;
			largestIndex = m_indexHeap[leftChild];
		}

		if (rightChild < size && Compare(m_content[largestIndex], m_content[m_indexHeap[rightChild]]))
		{
			largestChild = rightChild;
			largestIndex = m_indexHeap[rightChild];
		}

		if (largestChild != index)
		{
			Swap(&m_indexHeap[index], &m_indexHeap[largestChild]);
			Heapify(largestChild, size);
		}
	}

	template <typename T, typename Comparison>
	void BaldLion::Heap<T, Comparison>::Swap(ui32* a, ui32* b)
	{
		ui32 temp = *a;
		*a = *b;
		*b = temp;
	}

	template <typename T, typename Comparison>
	T BaldLion::Heap<T, Comparison>::Pop()
	{
		ui32 contentIndex = m_indexHeap[0];
		T value = m_content[contentIndex];

		m_content.RemoveAtFast(contentIndex);
		m_indexHeap.RemoveAtFast(0);

		BL_DYNAMICARRAY_FOREACH(m_indexHeap)
		{
			if (m_indexHeap[i] > contentIndex)
			{
				m_indexHeap[i]--;
			}
		}

		--m_size;

		Sort();
		return value;
	}

	template <typename T, typename Comparison>
	const T& BaldLion::Heap<T, Comparison>::Top() const
	{
		return m_content[m_indexHeap[0]];
	}

	template <typename T, typename Comparison>
	void BaldLion::Heap<T, Comparison>::Emplace(const T& value)
	{
		i32 indexOfValue = FindIndex(value);

		if (indexOfValue >= 0)
		{
			m_content[indexOfValue] = value;
		}
		else {
			m_content.PushBack(value);
			m_indexHeap.PushBack(m_content.Size() - 1);
			++m_size;
		}

		Sort();
	}

	template <typename T, typename Comparison /*= std::less<>*/>
	void BaldLion::Heap<T, Comparison>::Print()
	{
		BL_LOG_CORE_INFO("-------------------");
		BL_DYNAMICARRAY_FOREACH(m_indexHeap)
		{
			m_content[m_indexHeap[i]].Print();
		}
	}

}

