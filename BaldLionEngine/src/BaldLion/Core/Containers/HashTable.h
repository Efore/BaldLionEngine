#pragma once

#include "BaldLion/Core/Memory/MemoryManager.h"
#include "DynamicArray.h"

namespace std
{
	template <>
	struct hash<char *>
	{
		size_t operator()(char *s) const
		{
			size_t h = 5381;
			int c;
			while ((c = *s++))
				h = ((h << 5) + h) + c;
			return h;
		}
	};

	template <>
	struct hash<const char *>
	{
		size_t operator()(const char *s) const
		{
			size_t h = 5381;
			int c;
			while ((c = *s++))
				h = ((h << 5) + h) + c;
			return h;
		}
	};
}

namespace BaldLion
{
	template <typename V>
	struct HashNode {

		bool used = false;
		size_t hashedKey;
		V value; 

		HashNode::~HashNode() {
			value.~V();
		}
	};

	template <typename K, typename V, typename Allocator = Memory::Allocator>
	class HashTable
	{
		public:

			HashTable();
			HashTable(Memory::AllocationType allocationType, size_t capacity);

			bool Contains(const K& key) const;			

			void Insert(const K& key, V&& value);

			const V& Get(const K& key) const;
			V& Get(const K& key);

			bool Remove(const K& key);

			void Clear();
			void ClearNoDestructor();

			const size_t Size() const { return m_size; }

		private:						
			void Reallocate(size_t capacity);
			bool CheckContains(size_t hashedKey) const;

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
	void BaldLion::HashTable<K, V, Allocator>::Reallocate(size_t capacity)
	{
		DynamicArray<HashNode<V>> newTable = DynamicArray<HashNode<V>>(m_allocationType, capacity);
		newTable.Fill();

		for (size_t i = 0; i < m_table.Size(); ++i)
		{
			if (m_table[i].used)
			{
				size_t newIndex = m_table[i].hashedKey % capacity;

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

	template <typename K, typename V, typename Allocator /*= Memory::Allocator*/>
	bool BaldLion::HashTable<K, V, Allocator>::Contains(const K& key) const
	{
		const size_t hashedKey = std::hash<K>()(key);
		return CheckContains(hashedKey);
	}

	template <typename K, typename V, typename Allocator /*= Memory::Allocator*/>	
	bool BaldLion::HashTable<K, V, Allocator>::CheckContains(size_t hashedKey) const
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
	V& BaldLion::HashTable<K, V, Allocator>::Get(const K& key)
	{
		BL_ASSERT(Contains(key), "Key not contained");
		const size_t hashedKey = std::hash<K>()(key);

		size_t tableIndex = hashedKey % m_capacity;

		while (m_table[tableIndex].hashedKey != hashedKey)
		{
			tableIndex = (tableIndex + 1) % m_capacity;
		}

		return m_table[tableIndex].value;
	}

	template <typename K, typename V, typename Allocator /*= Memory::Allocator*/>
	const V& BaldLion::HashTable<K, V, Allocator>::Get(const K& key) const
	{
		BL_ASSERT(Contains(key), "Key not contained");
		const size_t hashedKey = std::hash<K>()(key);

		size_t tableIndex = hashedKey % m_capacity;

		while (m_table[tableIndex].hashedKey != hashedKey)
		{
			tableIndex = (tableIndex + 1) % m_capacity;
		}

		return m_table[tableIndex].value;
	}

	template <typename K, typename V, typename Allocator /*= Memory::Allocator*/>
	void BaldLion::HashTable<K, V, Allocator>::Insert(const K& key, V&& value)
	{
		const float ratio = (float)(++m_size) / (float)(m_capacity);
		if (ratio >= 0.5f)
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
		
		size_t index = tableIndex;

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