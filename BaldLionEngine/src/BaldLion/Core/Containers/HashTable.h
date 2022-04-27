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

			HashTableNode() = default;
		};

		struct Iterator
		{

		public:

			Iterator() = default;

			Iterator(HashTable<K,V>* hashTable, ui32 tableIndex ) : m_tableToIterate(&(hashTable->m_table)), m_tableIndex(tableIndex){				
			}

			Iterator& operator++() {
				
				BL_DYNAMICARRAY_FOR(i, (*m_tableToIterate), m_tableIndex + 1)
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

			bool operator== (const Iterator& other) const { return m_tableToIterate == other.m_tableToIterate && m_tableIndex == other.m_tableIndex; }
			bool operator!= (const Iterator& other) const { return m_tableToIterate != other.m_tableToIterate || m_tableIndex != other.m_tableIndex; }

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

			const ui32 GetTableIndex() const { return m_tableIndex; }

		private:

			ui32 m_tableIndex = 0;
			DynamicArray<HashTable::HashTableNode<K,V>>* m_tableToIterate = nullptr;
		};

		public:

			HashTable() = default;
			HashTable(Memory::AllocationType allocationType, ui32 capacity);

			bool Contains(const K& key) const;			

			void Emplace(const K& key, V&& value) noexcept;

			template <typename... Args >
			void Emplace(const K& key, Args&&... args) noexcept;

			bool TryGet(const K& key, V& result);
			bool TryGet(const K& key, V& result) const;

			bool TryGet(const K& key, V* result);
			bool TryGet(const K& key, V* result) const;

			const V& Get(const K& key) const;
			V& Get(const K& key);

			void Set(const K& key, V&& value) noexcept;
			void Set(const K& key, V value);

			bool Remove(const K& key);

			void Clear();

			void Delete();
			void DeleteNoDestructor();

			const ui32 Size() const { return m_size; }
			ui32 IndexOf(const K& key) const;

			HashTable<K,V>& operator= (const HashTable<K, V>& other);
			HashTable<K,V>& operator= (HashTable<K, V>&& other) noexcept;

			const Iterator& Begin() const;
			Iterator Begin();

			const Iterator& End() const;
			Iterator End();

		private:						
			void Reallocate(ui32 capacity);
			i32 FindIndex(hashType hashedKey) const;
			ui32 FindFirstElementIndex();

		private:

			ui32 m_capacity = 0;
			ui32 m_size = 0;

			DynamicArray<HashTableNode<K,V>> m_table;
			AllocationType m_allocationType = AllocationType::FreeList_Main;	

			HashTable<K, V>::Iterator m_beginIterator;
			HashTable<K, V>::Iterator m_endIterator;
	};

	template <typename K, typename V>
	BaldLion::HashTable<K, V>::HashTable(Memory::AllocationType allocationType, ui32 capacity) : m_size(0), m_capacity(capacity), m_allocationType(allocationType)
	{
		m_table = DynamicArray<HashTableNode<K, V>>(m_allocationType, capacity);
		m_table.Populate();
		
		m_beginIterator = HashTable<K, V>::Iterator(this, capacity);
		m_endIterator = HashTable<K, V>::Iterator(this, capacity);
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
		BL_DEEP_PROFILE_FUNCTION();

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
		BL_DEEP_PROFILE_FUNCTION();

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
	bool BaldLion::HashTable<K, V>::TryGet(const K& key, V* result)
	{
		BL_DEEP_PROFILE_FUNCTION();

		const hashType hashedKey = std::hash<K>()(key);
		const i32 tableIndex = FindIndex(hashedKey);

		if (tableIndex >= 0)
		{
			result = &(m_table[tableIndex].nodeValue);
			return true;
		}

		return false;
	}

	template <typename K, typename V>
	bool BaldLion::HashTable<K, V>::TryGet(const K& key, V* result) const
	{
		BL_DEEP_PROFILE_FUNCTION();

		const hashType hashedKey = std::hash<K>()(key);
		const i32 tableIndex = FindIndex(hashedKey);

		if (tableIndex >= 0)
		{
			result = dynamic_cast<V*>(const_cast<V*>(&(m_table[tableIndex].nodeValue)));
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
		BL_DEEP_PROFILE_FUNCTION();

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

		if (tableIndex < m_beginIterator.GetTableIndex())
			m_beginIterator = HashTable<K, V>::Iterator(this, (ui32)tableIndex); 

		m_endIterator = HashTable<K, V>::Iterator(this, m_capacity);
	}

	template <typename K, typename V>
	template <typename... Args >
	void BaldLion::HashTable<K, V>::Emplace(const K& key, Args&&... args) noexcept
	{
		BL_DEEP_PROFILE_FUNCTION();

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

		if (tableIndex < m_beginIterator.GetTableIndex())
			m_beginIterator = HashTable<K, V>::Iterator(this, (ui32)tableIndex);

		m_endIterator = HashTable<K, V>::Iterator(this, m_capacity);
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

		if (tableIndex == m_beginIterator.GetTableIndex())
		{
			m_beginIterator = HashTable<K, V>::Iterator(this, FindFirstElementIndex());
		}

		return true;
	}

	template <typename K, typename V>
	void BaldLion::HashTable<K, V>::Clear()
	{
		BL_DEEP_PROFILE_FUNCTION();

		BL_DYNAMICARRAY_FOR(i, m_table, 0)
		{
			m_table[i].used = false;
		}

		m_size = 0;

		m_beginIterator = HashTable<K, V>::Iterator(this, m_capacity);
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
		m_beginIterator = HashTable<K, V>::Iterator(this, FindFirstElementIndex());;
		m_endIterator = HashTable<K, V>::Iterator(this, m_capacity);

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
		m_beginIterator = HashTable<K, V>::Iterator(this, FindFirstElementIndex());;
		m_endIterator = HashTable<K, V>::Iterator(this, m_capacity);

		return *this;
	}

	template <typename K, typename V>
	typename const BaldLion::HashTable<K, V>::Iterator& BaldLion::HashTable<K, V>::Begin() const
	{
		return m_beginIterator;
	}

	template <typename K, typename V>
	typename BaldLion::HashTable<K, V>::Iterator BaldLion::HashTable<K, V>::Begin()
	{
		return m_beginIterator;
	}

	template <typename K, typename V>
	typename const BaldLion::HashTable<K, V> ::Iterator& BaldLion::HashTable<K, V>::End() const
	{
		return m_endIterator;
	}

	template <typename K, typename V>
	typename BaldLion::HashTable<K, V>::Iterator BaldLion::HashTable<K, V>::End()
	{
		return m_endIterator;
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
		newTable.Delete();
	}

	template <typename K, typename V>
	ui32 BaldLion::HashTable<K, V>::FindFirstElementIndex()
	{
		BL_DYNAMICARRAY_FOR(i, m_table, 0)
		{
			if (m_table[i].used)
			{
				return i;
			}
		}

		return m_capacity;
	}

	#define BL_HASHTABLE_FOR(tableToIterate, iteratorName) for(auto iteratorName = (tableToIterate).Begin(); iteratorName != (tableToIterate).End(); ++iteratorName)
}
