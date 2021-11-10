#pragma once

#include "BaldLion/Core/Memory/MemoryManager.h"
#include "DynamicArray.h"
#include "BaldLion/Core/Core.h"

namespace BaldLion
{
	template <typename K, typename V>
	class HashTable
	{ 
		public:

		template <typename K, typename V>
		struct HashTableNode {

			bool used = false;
			hashType hashedKey;
			K nodeKey;
			V nodeValue;

			HashTableNode() : nodeValue(V()) { }			
		};

		struct Iterator
		{

		public:

			Iterator(){}

			Iterator(HashTable<K,V>* hashTable, ui32 tableIndex ) : m_tableToIterate(&(hashTable->m_table)), m_tableIndex(tableIndex){				
			}

			Iterator& operator++() {
				
				for (ui32 i = m_tableIndex + 1; i < m_tableToIterate->Size(); ++i)
				{					
					if ((*m_tableToIterate)[i].used)
					{
						m_tableIndex = i;
						return *this;
					}
				}

				m_tableIndex = m_tableToIterate->Capacity();
				return *this;
			}

			friend bool operator == (const Iterator& a, const Iterator& b) { return a.m_tableToIterate == b.m_tableToIterate && a.m_tableIndex == b.m_tableIndex; }
			friend bool operator != (const Iterator& a, const Iterator& b) { return a.m_tableToIterate != b.m_tableToIterate || a.m_tableIndex != b.m_tableIndex; }

			Iterator& operator= (const Iterator& other)
			{
				if (&other == this)
					return *this;

				m_tableIndex = other.m_tableIndex;
				m_tableToIterate = other.m_tableToIterate;

				return *this;
			}
			
			const V& GetValue() const { return (*m_tableToIterate)[m_tableIndex].nodeValue; }
			V& GetValue() { return (*m_tableToIterate)[m_tableIndex].nodeValue; }

			const K& GetKey() const { return (*m_tableToIterate)[m_tableIndex].nodeKey; }			

		private:

			ui32 m_tableIndex = 0;
			DynamicArray<HashTable::HashTableNode<K,V>>* m_tableToIterate = nullptr;
		};

		public:

			HashTable();
			HashTable(Memory::AllocationType allocationType, ui32 capacity);

			bool Contains(const K& key) const;			

			void Emplace(const K& key, V&& value) noexcept;

			template <typename... Args >
			void Emplace(const K& key, Args&&... args) noexcept;

			bool TryGet(const K& key, V& result);
			bool TryGet(const K& key, V& result) const;

			const V& Get(const K& key) const;
			V& Get(const K& key);

			void Set(const K& key, V&& value) noexcept;
			void Set(const K& key, V value);

			bool Remove(const K& key);

			void Clear();
			void ClearNoDestructor();

			void Delete();
			void DeleteNoDestructor();

			const ui32 Size() const { return m_size; }
			ui32 IndexOf(const K& key) const;

			HashTable<K,V>& operator= (const HashTable<K, V>& other);
			HashTable<K,V>& operator= (HashTable<K, V>&& other) noexcept;

			Iterator Begin();

			Iterator End();

		private:						
			void Reallocate(ui32 capacity);
			i32 FindIndex(hashType hashedKey) const;
			ui32 FindFirstElementIndex();

		private:

			ui32 m_capacity = 0;
			ui32 m_size = 0;

			ui32 m_firstElementIndex = 0;

			DynamicArray<HashTableNode<K,V>> m_table;
			AllocationType m_allocationType;	
	};

	template <typename K, typename V>
	BaldLion::HashTable<K, V>::HashTable()
	{

	}

	template <typename K, typename V>
	BaldLion::HashTable<K, V>::HashTable(Memory::AllocationType allocationType, ui32 capacity) : m_size(0), m_capacity(capacity), m_allocationType(allocationType)
	{
		m_table = DynamicArray<HashTableNode<K, V>>(m_allocationType, capacity);
		m_table.Populate();

		m_firstElementIndex = capacity;
	}

	template <typename K, typename V>
	bool BaldLion::HashTable<K, V>::Contains(const K& key) const
	{
		const hashType hashedKey = std::hash<K>()(key);
		return FindIndex(hashedKey) != -1;
	}

	template <typename K, typename V>	
	i32 BaldLion::HashTable<K, V>::FindIndex(hashType hashedKey) const
	{
		const hashType tableIndex = hashedKey % m_capacity;

		hashType index = tableIndex;

		while (index < m_capacity)
		{
			if (m_table[index].hashedKey == hashedKey && m_table[index].used)
			{
				return (i32)index;
			}
			else
			{
				++index;
			}
		}

		index = 0;
		while (index < tableIndex)
		{
			if (m_table[index].hashedKey == hashedKey && m_table[index].used)
			{
				return (i32)index;
			}
			else
			{
				++index;
			}
		}

		return -1;
	}

	template <typename K, typename V>
	bool BaldLion::HashTable<K, V>::TryGet(const K& key, V& result)
	{
		const hashType hashedKey = std::hash<K>()(key);
		const i32 tableIndex = FindIndex(hashedKey);

		if (tableIndex >= 0) 
		{
			result = m_table[tableIndex].nodeValue;
			return true;
		}

		return false;
	}

	template <typename K, typename V>
	bool BaldLion::HashTable<K, V>::TryGet(const K& key, V& result) const
	{
		const hashType hashedKey = std::hash<K>()(key);
		const i32 tableIndex = FindIndex(hashedKey);

		if (tableIndex >= 0)
		{
			result = m_table[tableIndex].nodeValue;
			return true;
		}

		return false;
	}

	template <typename K, typename V>
	V& BaldLion::HashTable<K, V>::Get(const K& key)
	{
		const hashType hashedKey = std::hash<K>()(key);
		const i32 tableIndex = FindIndex(hashedKey);

		BL_ASSERT(tableIndex >= 0, "Key not contained");

		return m_table[tableIndex].nodeValue;
	}

	template <typename K, typename V>
	const V& BaldLion::HashTable<K, V>::Get(const K& key) const
	{
		const hashType hashedKey = std::hash<K>()(key);
		const i32 tableIndex = FindIndex(hashedKey);

		BL_ASSERT(tableIndex >= 0, "Key not contained");

		return m_table[tableIndex].nodeValue;
	}
	
	template <typename K, typename V>
	void BaldLion::HashTable<K, V>::Set(const K& key, V&& value) noexcept
	{
		const hashType hashedKey = std::hash<K>()(key);
		const i32 tableIndex = FindIndex(hashedKey);

		BL_ASSERT(tableIndex >= 0, "Key not contained");

		m_table[tableIndex].nodeValue = std::move(value);
	}

	template <typename K, typename V>
	void BaldLion::HashTable<K, V>::Set(const K& key, V value)
	{
		const hashType hashedKey = std::hash<K>()(key);
		const i32 tableIndex = FindIndex(hashedKey);

		BL_ASSERT(tableIndex >= 0, "Key not contained");

		m_table[tableIndex].nodeValue = value;
	}

	template <typename K, typename V>
	void BaldLion::HashTable<K, V>::Emplace(const K& key, V&& value) noexcept
	{
		const float ratio = (float)(++m_size) / (float)(m_capacity);

		if (ratio > 0.75f)
		{
			//BL_LOG_CORE_INFO("Need reallocation");
			Reallocate((ui32)(m_capacity * 1.5f));
		}

		const hashType hashedKey = std::hash<K>()(key);
		hashType tableIndex = hashedKey % m_capacity;

		while (m_table[tableIndex].used)
		{
			tableIndex = (tableIndex + 1) % m_capacity;
		}

		//BL_LOG_CORE_INFO("found free node");

		HashTableNode<K,V>& node = m_table[tableIndex];
		node.used = true;
		node.hashedKey = hashedKey;
		node.nodeKey = key;
		node.nodeValue = std::move(value);

		if (m_firstElementIndex == -1 || tableIndex < m_firstElementIndex)
			m_firstElementIndex = (i32)tableIndex;
	}

	template <typename K, typename V>
	template <typename... Args >
	void BaldLion::HashTable<K, V>::Emplace(const K& key, Args&&... args) noexcept
	{
		const float ratio = (float)(++m_size) / (float)(m_capacity);

		if (ratio > 0.75f)
		{
			//BL_LOG_CORE_INFO("Need reallocation");
			Reallocate((ui32)(m_capacity * 1.5f));
		}

		const hashType hashedKey = std::hash<K>()(key);
		hashType tableIndex = hashedKey % m_capacity;

		while (m_table[tableIndex].used)
		{
			tableIndex = (tableIndex + 1) % m_capacity;
		}

		//BL_LOG_CORE_INFO("found free node");

		HashTableNode<K, V>& node = m_table[tableIndex];
		node.used = true;
		node.hashedKey = hashedKey;
		node.nodeKey = key;

		new (&node.nodeValue) V(std::forward<Args>(args)...);

		if (m_firstElementIndex == -1 || tableIndex < m_firstElementIndex)
			m_firstElementIndex = (i32)tableIndex;
	}

	template <typename K, typename V>
	bool BaldLion::HashTable<K, V>::Remove(const K& key)
	{
		const hashType hashedKey = std::hash<K>()(key);
		const i32 tableIndex = FindIndex(hashedKey);

		if (tableIndex < 0)
			return false;
		
		m_table[tableIndex].used = false;		
		m_table[tableIndex].nodeValue.~V();
		--m_size;

		if (tableIndex == m_firstElementIndex)
		{
			m_firstElementIndex = FindFirstElementIndex();
		}

		return true;
	}

	template <typename K, typename V>
	void BaldLion::HashTable<K, V>::Clear()
	{
		for (ui32 i = 0; i < m_table.Size(); ++i)
		{
			m_table[i].used = false;
			m_table[i].nodeValue.~V();
		}

		m_size = 0;

		m_firstElementIndex = m_capacity;
	}

	template <typename K, typename V>
	void BaldLion::HashTable<K, V>::ClearNoDestructor()
	{
		for (ui32 i = 0; i < m_table.Size(); ++i)
		{
			m_table[i].used = false;
		}

		m_size = 0;

		m_firstElementIndex = m_capacity;
	}

	template <typename K, typename V>
	void BaldLion::HashTable<K, V>::DeleteNoDestructor()
	{
		m_table.DeleteNoDestructor();
		m_capacity = 0;
		m_size = 0;
	}

	template <typename K, typename V>
	void BaldLion::HashTable<K, V>::Delete()
	{
		m_table.Delete(); 
		m_capacity = 0;
		m_size = 0;
	}

	template <typename K, typename V>
	ui32 BaldLion::HashTable<K, V>::IndexOf(const K& key) const
	{
		const hashType hashedKey = std::hash<K>()(key);
		const i32 tableIndex = FindIndex(hashedKey);

		BL_ASSERT(tableIndex >= 0, "Key not contained");

		return (ui32)tableIndex;
	}

	template <typename K, typename V>
	BaldLion::HashTable<K, V>& BaldLion::HashTable<K, V>::operator=(HashTable<K, V>&& other) noexcept
	{
		if (&other == this)
			return *this;

		Delete();

		m_allocationType = other.m_allocationType;
		m_size = other.m_size;
		m_capacity = other.m_capacity;
		m_table = std::move(other.m_table);		
		m_firstElementIndex = other.m_firstElementIndex;

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
		m_table = DynamicArray<HashTableNode<K, V>>(other.m_table);
		m_firstElementIndex = other.m_firstElementIndex;

		return *this;
	}

	template <typename K, typename V>
	typename BaldLion::HashTable<K, V>::Iterator BaldLion::HashTable<K, V>::Begin()
	{
		return HashTable<K, V>::Iterator(this, m_firstElementIndex);
	}

	template <typename K, typename V>
	typename BaldLion::HashTable<K, V> ::Iterator BaldLion::HashTable<K, V>::End()
	{
		return HashTable<K,V>::Iterator(this, m_capacity);
	}

	template <typename K, typename V>
	void BaldLion::HashTable<K, V>::Reallocate(ui32 capacity)
	{
		DynamicArray<HashTableNode<K, V>> newTable = DynamicArray<HashTableNode<K, V>>(m_allocationType, capacity);
		newTable.Populate();

		for (ui32 i = 0; i < m_table.Size(); ++i)
		{
			if (m_table[i].used)
			{
				hashType newIndex = m_table[i].hashedKey % capacity;

				while (newTable[newIndex].used)
				{
					newIndex = (newIndex + 1) % capacity;
				}

				newTable[newIndex] = m_table[i];
			}
		}

		m_table.Delete();
		m_table = std::move(newTable);
		m_capacity = capacity;
		m_firstElementIndex = FindFirstElementIndex();

		newTable.Delete();
	}

	template <typename K, typename V>
	ui32 BaldLion::HashTable<K, V>::FindFirstElementIndex()
	{
		for (ui32 i = 0; i < m_table.Size(); ++i)
		{
			if (m_table[i].used)
			{
				return i;
			}
		}

		return m_capacity;
	}
}
