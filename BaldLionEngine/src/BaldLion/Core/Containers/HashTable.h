#pragma once

#include "BaldLion/Core/Memory/MemoryManager.h"
#include "DynamicArray.h"
#include "BaldLion/Core/Core.h"

namespace BaldLion
{
	template <typename V>
	struct HashNode {

		bool used = false;
		hashV hashedKey;
		V value; 

		HashNode::~HashNode() {
			value.~V();
		}
	};

	template <typename K, typename V>
	class HashTable
	{
		public:

			HashTable();
			HashTable(Memory::AllocationType allocationType, ui32 capacity);

			bool Contains(const K& key) const;			

			void Insert(const K& key, V&& value) noexcept;

			const V& Get(const K& key) const;
			V& Get(const K& key);

			void Set(const K& key, V&& value) noexcept;
			void Set(const K& key, V value);

			bool Remove(const K& key);

			void Clear();
			void ClearNoDestructor();

			const ui32 Size() const { return m_size; }
			ui32 IndexOf(const K& key) const;

			HashTable<K,V>& operator= (const HashTable<K, V>& other);
			HashTable<K,V>& operator= (HashTable<K, V>&& other) noexcept;

		private:						
			void Reallocate(ui32 capacity);
			bool CheckContains(hashV hashedKey) const;

		private:

			ui32 m_capacity = 0;
			ui32 m_size = 0;

			DynamicArray<HashNode<V>> m_table;
			AllocationType m_allocationType;	
	};

	

	template <typename K, typename V>
	BaldLion::HashTable<K, V>::HashTable()
	{

	}

	template <typename K, typename V>
	BaldLion::HashTable<K, V>::HashTable(Memory::AllocationType allocationType, ui32 capacity) : m_size(0), m_capacity(capacity), m_allocationType(allocationType)
	{
		m_table = DynamicArray<HashNode<V>>(m_allocationType, capacity);
		m_table.Fill();
	}

	template <typename K, typename V>
	bool BaldLion::HashTable<K, V>::Contains(const K& key) const
	{
		const hashV hashedKey = std::hash<K>()(key);
		return CheckContains(hashedKey);
	}

	template <typename K, typename V>	
	bool BaldLion::HashTable<K, V>::CheckContains(hashV hashedKey) const
	{
		const hashV tableIndex = hashedKey % m_capacity;

		hashV index = tableIndex;

		while (index < m_capacity)
		{
			if (m_table[index].hashedKey == hashedKey)
			{
				return true;
			}
			else
			{
				++index;
			}
		}

		index = 0;
		while (index < tableIndex)
		{
			if (m_table[index].hashedKey == hashedKey)
			{
				return true;
			}
			else
			{
				++index;
			}
		}

		return false;
	}

	template <typename K, typename V>
	V& BaldLion::HashTable<K, V>::Get(const K& key)
	{
		BL_ASSERT(Contains(key), "Key not contained");

		const hashV hashedKey = std::hash<K>()(key);

		hashV tableIndex = hashedKey % m_capacity;

		while (m_table[tableIndex].hashedKey != hashedKey)
		{
			tableIndex = (tableIndex + 1) % m_capacity;
		}

		return m_table[tableIndex].value;
	}

	template <typename K, typename V>
	const V& BaldLion::HashTable<K, V>::Get(const K& key) const
	{
		BL_ASSERT(Contains(key), "Key not contained");
		
		const hashV hashedKey = std::hash<K>()(key);

		hashV tableIndex = hashedKey % m_capacity;

		while (m_table[tableIndex].hashedKey != hashedKey)
		{
			tableIndex = (tableIndex + 1) % m_capacity;
		}

		return m_table[tableIndex].value;
	}
	
	template <typename K, typename V>
	void BaldLion::HashTable<K, V>::Set(const K& key, V&& value) noexcept
	{
		BL_ASSERT(Contains(key), "Key not contained");

		const hashV hashedKey = std::hash<K>()(key);

		hashV tableIndex = hashedKey % m_capacity;

		while (m_table[tableIndex].hashedKey != hashedKey)
		{
			tableIndex = (tableIndex + 1) % m_capacity;
		}

		m_table[tableIndex].value = std::move(value);
	}

	template <typename K, typename V>
	void BaldLion::HashTable<K, V>::Set(const K& key, V value)
	{
		BL_ASSERT(Contains(key), "Key not contained");

		const hashV hashedKey = std::hash<K>()(key);

		hashV tableIndex = hashedKey % m_capacity;

		while (m_table[tableIndex].hashedKey != hashedKey)
		{
			tableIndex = (tableIndex + 1) % m_capacity;
		}

		m_table[tableIndex].value = value;
	}

	template <typename K, typename V>
	void BaldLion::HashTable<K, V>::Insert(const K& key, V&& value) noexcept
	{
		const float ratio = (float)(++m_size) / (float)(m_capacity);
		if (ratio > 0.5f)
		{
			Reallocate((ui32)(m_capacity * 1.5f));
		}

		const hashV hashedKey = std::hash<K>()(key);
		hashV tableIndex = hashedKey % m_capacity;

		while (m_table[tableIndex].used)
		{
			tableIndex = (tableIndex + 1) % m_capacity;
		}

		m_table[tableIndex].used = true;
		m_table[tableIndex].hashedKey = hashedKey;
		m_table[tableIndex].value = std::move(value);
	}

	template <typename K, typename V>
	bool BaldLion::HashTable<K, V>::Remove(const K& key)
	{
		if (!Contains(key))
			return false;

		const hashV hashedKey = std::hash<K>()(key);
		const hashV tableIndex = hashedKey % m_capacity;
		
		hashV index = tableIndex;

		while (index < m_capacity )
		{
			if (m_table[index].hashedKey == hashedKey)
			{
				m_table[tableIndex].used = false;
				m_table[tableIndex].value.~V();
				--m_size;
				return true;
			}
			else
			{
				++index;
			}
		}

		index = 0;
		while (index < tableIndex)
		{
			if (m_table[index].hashedKey == hashedKey)
			{
				m_table[tableIndex].used = false;
				m_table[tableIndex].value.~V();
				--m_size;
				return true;
			}
			else
			{
				++index;
			}
		}

		return false;
	}

	template <typename K, typename V>
	void BaldLion::HashTable<K, V>::ClearNoDestructor()
	{
		m_table.ClearNoDestructor();
		m_capacity = 0;
		m_size = 0;
	}

	template <typename K, typename V>
	void BaldLion::HashTable<K, V>::Clear()
	{
		m_table.Clear(); 
		m_capacity = 0;
		m_size = 0;
	}

	template <typename K, typename V>
	ui32 BaldLion::HashTable<K, V>::IndexOf(const K& key) const
	{
		BL_ASSERT(Contains(key), "Key not contained");

		const hashV hashedKey = std::hash<K>()(key);

		ui32 currentIndex = 0;
		for (ui32 i = 0; i < m_table.Size(); ++i)
		{
			if (m_table[i].hashedKey == hashedKey)
				return currentIndex;

			if (m_table[i].used)
				++currentIndex;
		}
	}

	template <typename K, typename V>
	BaldLion::HashTable<K, V>& BaldLion::HashTable<K, V>::operator=(HashTable<K, V>&& other) noexcept
	{
		if (&other == this)
			return *this;

		Clear();

		m_allocationType = other.m_allocationType;
		m_size = other.m_size;
		m_capacity = other.m_capacity;
		m_table = std::move(other.m_table);		

		return *this;
	}

	template <typename K, typename V>
	BaldLion::HashTable<K, V>& BaldLion::HashTable<K, V>::operator=(const HashTable<K, V>& other)
	{
		if (&other == this)
			return *this;

		m_allocationType = other.m_allocationType;
		m_size = other.m_size;
		m_capacity = other.m_capacity;
		m_table = DynamicArray<HashNode<V>>(other.m_table);

		return *this;
	}

	template <typename K, typename V>
	void BaldLion::HashTable<K, V>::Reallocate(ui32 capacity)
	{
		DynamicArray<HashNode<V>> newTable = DynamicArray<HashNode<V>>(m_allocationType, capacity);
		newTable.Fill();

		for (ui32 i = 0; i < m_table.Size(); ++i)
		{
			if (m_table[i].used)
			{
				hashV newIndex = m_table[i].hashedKey % capacity;

				while (newTable[newIndex].used)
				{
					newIndex = (newIndex + 1) % capacity;
				}

				newTable[newIndex] = m_table[i];
			}
		}

		m_table.Clear();
		m_table = DynamicArray<HashNode<V>>(newTable);
		m_capacity = capacity;

		newTable.Clear();
	}
}
