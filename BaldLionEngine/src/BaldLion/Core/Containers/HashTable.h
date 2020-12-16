#pragma once

#include "BaldLion/Core/Memory/MemoryManager.h"
#include "DynamicArray.h"


namespace BaldLion
{
	template <typename V>
	struct HashNode {

		bool used = false;
		size_t hashedKey;
		V value;
	};

	template <typename K, typename V, typename Allocator = Memory::Allocator>
	class HashTable
	{
		public:

			struct Proxy {

				HashTable& m_tableContainer;
				size_t m_hashedKey;	
				size_t m_tableIndex;

				Proxy(HashTable& t, size_t hashedKey) : m_tableContainer(t), m_hashedKey(hashedKey), m_tableIndex(m_hashedKey %  m_tableContainer.m_capacity) {}

				operator V&() { return m_tableContainer.m_table[m_tableIndex].value; }
				operator const V&() const { return m_tableContainer.m_table[m_tableIndex].value; }

				V& operator= (V&& other) { 					

					if (!m_tableContainer.Contains(m_hashedKey))
					{
						if ((float)(m_tableContainer.m_size++) / (float)(m_tableContainer.m_capacity) > 0.5f)
						{
							m_tableContainer.Reallocate((size_t)(m_tableContainer.m_capacity * 1.5f));
						}						

						while (m_tableContainer.m_table[m_tableIndex].used)
						{
							m_tableIndex = (m_tableIndex + 1) % m_tableContainer.m_capacity;
						}
					
						m_tableContainer.m_table[m_tableIndex].used = true;
						m_tableContainer.m_table[m_tableIndex].hashedKey = m_hashedKey;
					}

					return m_tableContainer.m_table[m_tableIndex].value = std::move(other);				
				}
			};

		
		public:

			HashTable();
			HashTable(Memory::AllocationType allocationType, size_t capacity);

			bool Contains(const K& key) const;
			
			Proxy operator[](const K& key);		

			void Insert(const K& key, V&& value);
			bool Remove(const K& key);

			void Clear();
			void ClearNoDestructor();

		private:						
			void Reallocate(size_t capacity);
			bool Contains(size_t hashedKey);

		private:

			size_t m_capacity = 0;
			size_t m_size = 0;
			DynamicArray<HashNode<V>> m_table;
			AllocationType m_allocationType;	
	};

	template <typename K, typename V, typename Allocator /*= Memory::Allocator*/>
	BaldLion::HashTable<K, V, Allocator>::HashTable()
	{

	}

	template <typename K, typename V, typename Allocator /*= Memory::Allocator*/>
	BaldLion::HashTable<K, V, Allocator>::HashTable(Memory::AllocationType allocationType, size_t capacity) : m_size(0), m_capacity(capacity), m_allocationType(allocationType)
	{
		m_table = DynamicArray<HashNode<V>>(m_allocationType, capacity);
		m_table.Fill();
	}

	template <typename K, typename V, typename Allocator /*= Memory::Allocator*/>
	typename BaldLion::HashTable<K, V, Allocator>::Proxy BaldLion::HashTable<K, V, Allocator>::operator[](const K& key)
	{
		const size_t hashedKey = std::hash<K>()(key);

		return Proxy(*this, hashedKey);
	}

	template <typename K, typename V, typename Allocator /*= Memory::Allocator*/>
	void BaldLion::HashTable<K, V, Allocator>::Reallocate(size_t capacity)
	{
		DynamicArray<HashNode<V>> newTable = DynamicArray<HashNode<V>>(m_allocationType, capacity);
		newTable.Fill();

		for (int i = 0; i < m_table.Size(); ++i)
		{
			if (m_table[i].used)
			{
				int newIndex = m_table[i].hashedKey % capacity;

				while (newTable[newIndex].used)
				{
					newIndex = (newIndex + 1) % capacity;
				}
				
				newTable[newIndex] = m_table[i];
			}
		}

		m_table.Clear();
		m_table = DynamicArray<HashNode<V>>(newTable);
		newTable.Clear();
	}	

	template <typename K, typename V, typename Allocator /*= Memory::Allocator*/>
	bool BaldLion::HashTable<K, V, Allocator>::Contains(const K& key) const
	{
		const size_t hashedKey = std::hash<K>()(key);
		return Contains(hashedKey);
	}

	template <typename K, typename V, typename Allocator /*= Memory::Allocator*/>
	bool BaldLion::HashTable<K, V, Allocator>::Contains(size_t hashedKey)
	{
		const size_t tableIndex = hashedKey % m_capacity;

		size_t index = tableIndex;

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

	template <typename K, typename V, typename Allocator /*= Memory::Allocator*/>
	void BaldLion::HashTable<K, V, Allocator>::Insert(const K& key, V&& value)
	{
		const float ratio = (float)(++m_size) / (float)(m_capacity);
		if (ratio > 0.5f)
		{
			Reallocate((size_t)(m_capacity * 1.5f));
		}

		const size_t hashedKey = std::hash<K>()(key);
		size_t tableIndex = hashedKey % m_capacity;

		while (m_table[tableIndex].used)
		{
			tableIndex = (tableIndex + 1) % m_capacity;
		}

		m_table[tableIndex].used = true;
		m_table[tableIndex].hashedKey = hashedKey;
		m_table[tableIndex].value = std::move(value);
	}

	template <typename K, typename V, typename Allocator /*= Memory::Allocator*/>
	bool BaldLion::HashTable<K, V, Allocator>::Remove(const K& key)
	{
		if (!Contains(key))
			return false;

		const size_t hashedKey = = std::hash<K>()(key);
		const size_t tableIndex = hashedKey % m_capacity;

		
		int index = tableIndex;

		while (index < m_capacity )
		{
			if (m_table[index].hashedKey == hashedKey)
			{
				m_table[tableIndex].used = false;
				m_table[tableIndex].value.~V();
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
				return true;
			}
			else
			{
				++index;
			}
		}

		return false;
	}

	template <typename K, typename V, typename Allocator /*= Memory::Allocator*/>
	void BaldLion::HashTable<K, V, Allocator>::ClearNoDestructor()
	{
		m_table.ClearNoDestructor();
	}

	template <typename K, typename V, typename Allocator /*= Memory::Allocator*/>
	void BaldLion::HashTable<K, V, Allocator>::Clear()
	{
		m_table.Clear(); 
	}

}
