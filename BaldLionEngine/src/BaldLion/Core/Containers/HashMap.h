#pragma once

#include "BaldLion/Core/Memory/MemoryManager.h"
#include "DynamicArray.h"
#include "BaldLion/Core/Core.h"

namespace BaldLion
{
	template <typename K, typename V>
	class HashMap
	{ 
		public:

		template <typename K, typename V>
		struct HashMapNode {
			
			hashType hashedMapKey;

			K nodeKey;
			V nodeValue;

			HashMapNode<K, V>* nextNode;

			HashMapNode(const hashType hashedKey, const K& key) : hashedMapKey(hashedKey), nodeKey(key), nextNode(nullptr) { }
			HashMapNode(const hashType hashedKey, const K& key, V&& value) : hashedMapKey(hashedKey), nodeKey(key), nodeValue(std::move(value)), nextNode(nullptr)  { }
		};

		struct Iterator
		{

		public:

			Iterator(){}

			Iterator(HashMap<K,V>* hashTable, ui32 tableIndex, HashMapNode<K, V>* currentNode) : m_tableToIterate(&(hashTable->m_table)), m_tableIndex(tableIndex), m_currentNode(currentNode) {}

			Iterator& operator++() {
				
				if (m_currentNode != nullptr && m_currentNode->nextNode != nullptr)
				{
					m_currentNode = m_currentNode->nextNode;
					return *this;
				}

				for (ui32 i = m_tableIndex + 1; i < m_tableToIterate->Size();)
				{		
					if ((*m_tableToIterate)[i] == nullptr)
					{
						i++;
					}
					else
					{
						m_tableIndex = i;
						m_currentNode = (*m_tableToIterate)[i];
						return *this;
					}
				}

				m_tableIndex = m_tableToIterate->Capacity();
				m_currentNode = nullptr;

				return *this;
			}

			friend bool operator == (const Iterator& a, const Iterator& b) { return a.m_tableToIterate == b.m_tableToIterate && a.m_tableIndex == b.m_tableIndex && a.m_currentNode == b.m_currentNode; }
			friend bool operator != (const Iterator& a, const Iterator& b) { return a.m_tableToIterate != b.m_tableToIterate || a.m_tableIndex != b.m_tableIndex || a.m_currentNode != b.m_currentNode; }

			Iterator& operator= (const Iterator& other)
			{
				if (&other == this)
					return *this;

				m_tableIndex = other.m_tableIndex;
				m_tableToIterate = other.m_tableToIterate;
				m_currentNode = other.m_currentNode;

				return *this;
			}
			
			const V& GetValue() const { return m_currentNode->nodeValue; }
			V& GetValue() { return m_currentNode->nodeValue; }

			const K& GetKey() const { return m_currentNode->nodeKey; }

		private:

			ui32 m_tableIndex = 0;
			HashMapNode<K, V>* m_currentNode = nullptr;
			DynamicArray<HashMapNode<K,V>*>* m_tableToIterate = nullptr;
		};

		public:

			HashMap();
			HashMap(Memory::AllocationType allocationType, ui32 capacity);

			bool Contains(const K& key) const;			

			void Emplace(const K& key, V&& value) noexcept;

			template <typename... Args >
			void Emplace(const K& key, Args&&... args) noexcept;

			bool TryGet(const K& key, V& result);
			const V& Get(const K& key) const;
			V& Get(const K& key);

			void Set(const K& key, V&& value) noexcept;
			void Set(const K& key, V value);

			bool Remove(const K& key);

			void Delete();
			void DeleteNoDestructor();

			const ui32 Size() const { return m_size; }
			ui32 IndexOf(const K& key) const;

			HashMap<K,V>& operator= (const HashMap<K, V>& other);
			HashMap<K,V>& operator= (HashMap<K, V>&& other) noexcept;

			Iterator Begin();
			Iterator End();

		private:						
			void Reallocate(ui32 capacity);
			const HashMapNode<K,V>* FindNode(hashType hashedMapKey) const;
			HashMapNode<K,V>* FindNode(hashType hashedMapKey);
			ui32 FindFirstElementIndex();

		private:

			ui32 m_capacity = 0;
			ui32 m_size = 0;

			ui32 m_firstElementIndex = 0;
			ui32 m_lastElementIndex = 0;

			DynamicArray<HashMapNode<K,V>*> m_table;
			AllocationType m_allocationType;	
	};

	template <typename K, typename V>
	BaldLion::HashMap<K, V>::HashMap()
	{

	}

	template <typename K, typename V>
	BaldLion::HashMap<K, V>::HashMap(Memory::AllocationType allocationType, ui32 capacity) : m_size(0), m_capacity(capacity), m_allocationType(allocationType)
	{
		m_table = DynamicArray<HashMapNode<K, V>*>(m_allocationType, capacity);
		m_table.Populate(nullptr);

		m_firstElementIndex = capacity;
		m_lastElementIndex = capacity;
	}

	template <typename K, typename V>
	bool BaldLion::HashMap<K, V>::Contains(const K& key) const
	{
		const hashType hashedMapKey = std::hash<K>()(key);
		return FindNode(hashedMapKey) != nullptr;
	}

	template <typename K, typename V>	
	const BaldLion::HashMap<K, V>::HashMapNode<K, V>* BaldLion::HashMap<K, V>::FindNode(hashType hashedMapKey) const
	{
		const hashType tableIndex = hashedMapKey % m_capacity;

		if (m_table[tableIndex] != nullptr)
		{
			HashMapNode<K, V>* nodeIterator = m_table[tableIndex];

			while (nodeIterator != nullptr && nodeIterator->hashedMapKey != hashedMapKey)
			{
				nodeIterator = nodeIterator->nextNode;
			}

			return nodeIterator;
		}

		return nullptr;
	}

	template <typename K, typename V>
	BaldLion::HashMap<K, V>::HashMapNode<K, V>* BaldLion::HashMap<K, V>::FindNode(hashType hashedMapKey)
	{
		const hashType tableIndex = hashedMapKey % m_capacity;

		if (m_table[tableIndex] != nullptr)
		{
			HashMapNode<K, V>* nodeIterator = m_table[tableIndex];

			while (nodeIterator != nullptr && nodeIterator->hashedMapKey != hashedMapKey)
			{
				nodeIterator = nodeIterator->nextNode;
			}

			return nodeIterator;
		}

		return nullptr;
	}

	template <typename K, typename V>
	bool BaldLion::HashMap<K, V>::TryGet(const K& key, V& result)
	{
		const hashType hashedMapKey = std::hash<K>()(key);
		HashMapNode<K, V>* node = FindNode(hashedMapKey);

		if (node != nullptr)
		{
			result = node->nodeValue;
			return true;
		}

		return false;
	}

	template <typename K, typename V>
	V& BaldLion::HashMap<K, V>::Get(const K& key)
	{
		const hashType hashedMapKey = std::hash<K>()(key);
		HashMapNode<K, V>* node = FindNode(hashedMapKey);

		BL_ASSERT(node, "Key not contained");		
		return node->nodeValue;
	}

	template <typename K, typename V>
	const V& BaldLion::HashMap<K, V>::Get(const K& key) const
	{
		const hashType hashedMapKey = std::hash<K>()(key);
		const HashMapNode<K, V>* node = FindNode(hashedMapKey);

		BL_ASSERT(node, "Key not contained");
		return node->nodeValue;
	}
	
	template <typename K, typename V>
	void BaldLion::HashMap<K, V>::Set(const K& key, V&& value) noexcept
	{
		const hashType hashedMapKey = std::hash<K>()(key);
		HashMapNode<K, V>* node = FindNode(hashedMapKey);

		BL_ASSERT(node, "Key not contained");
		node->nodeValue = std::move(value);
	}

	template <typename K, typename V>
	void BaldLion::HashMap<K, V>::Set(const K& key, V value)
	{
		const hashType hashedMapKey = std::hash<K>()(key);
		HashMapNode<K, V>* node = FindNode(hashedMapKey);

		BL_ASSERT(node, "Key not contained");
		node->nodeValue = value;
	}

	template <typename K, typename V>
	void BaldLion::HashMap<K, V>::Emplace(const K& key, V&& value) noexcept
	{
		const float ratio = (float)(++m_size) / (float)(m_capacity);

		if (ratio > 0.75f)
		{
			//BL_LOG_CORE_INFO("Need reallocation");
			Reallocate((ui32)(m_capacity * 1.5f));
			m_lastElementIndex = m_table.Capacity();
		}

		const hashType hashedMapKey = std::hash<K>()(key);

		HashMapNode<K, V>* newNode = MemoryManager::New<HashMapNode<K, V>>("HashMapNode", m_allocationType, hashedMapKey, key, std::move(value));

		const hashType tableIndex = hashedMapKey % m_capacity;

		if (m_table[tableIndex] == nullptr)
		{
			m_table[tableIndex] = newNode;
		}
		else {

			HashMapNode<K, V>* nodeIterator = m_table[tableIndex];

			while (nodeIterator->nextNode != nullptr)
			{
				nodeIterator = nodeIterator->nextNode;
			}

			nodeIterator->nextNode = newNode;
		}

		if (m_firstElementIndex == -1 || tableIndex < m_firstElementIndex)
			m_firstElementIndex = (i32)tableIndex;
	}

	template <typename K, typename V>
	template <typename... Args >
	void BaldLion::HashMap<K, V>::Emplace(const K& key, Args&&... args) noexcept
	{
		const float ratio = (float)(++m_size) / (float)(m_capacity);

		if (ratio > 0.75f)
		{
			//BL_LOG_CORE_INFO("Need reallocation");
			Reallocate((ui32)(m_capacity * 1.5f));
			m_lastElementIndex = m_table.Capacity();
		}

		const hashType hashedMapKey = std::hash<K>()(key);

		HashMapNode<K, V>* newNode = MemoryManager::New<HashMapNode<K, V>>("HashMapNode", m_allocationType, hashedMapKey, key);
		new (&(newNode->nodeValue)) V(std::forward<Args>(args)...);

		const hashType tableIndex = hashedMapKey % m_capacity;

		if (m_table[tableIndex] == nullptr)
		{
			m_table[tableIndex] = newNode;
		}
		else {

			HashMapNode<K, V>* nodeIterator = m_table[tableIndex];

			while (nodeIterator->nextNode != nullptr)
			{
				nodeIterator = nodeIterator->nextNode;
			}

			nodeIterator->nextNode = newNode;
		}

		if (m_firstElementIndex == -1 || tableIndex < m_firstElementIndex)
			m_firstElementIndex = (i32)tableIndex;
	}


	template <typename K, typename V>
	bool BaldLion::HashMap<K, V>::Remove(const K& key)
	{
		const hashType hashedMapKey = std::hash<K>()(key);

		HashMapNode<K, V>* node = FindNode(hashedMapKey);

		if (!node)
			return false;

		const hashType tableIndex = hashedMapKey % m_capacity;

		if (m_table[tableIndex] == node)
		{
			m_table[tableIndex] = node->nextNode;
		}
		else
		{
			HashMapNode<K, V>* prevNodeIterator = m_table[tableIndex];

			while (prevNodeIterator->nextNode != node)
			{
				prevNodeIterator = prevNodeIterator->nextNode;
			}

			prevNodeIterator->nextNode = node->nextNode;
		}

		MemoryManager::Delete(node);

		--m_size;

		if (tableIndex == m_firstElementIndex)
		{
			m_firstElementIndex = FindFirstElementIndex();
		}

		m_lastElementIndex = m_table.Capacity();

		return true;
	}

	template <typename K, typename V>
	void BaldLion::HashMap<K, V>::DeleteNoDestructor()
	{
		for (ui32 i = 0; i < m_table.Size(); ++i)
		{
			HashMapNode<K, V>* nodeIterator = m_table[i];

			while (nodeIterator != nullptr)
			{
				HashMapNode<K, V>* temp = nodeIterator->nextNode;

				MemoryManager::DeleteNoDestructor(nodeIterator);

				nodeIterator = temp;
			}
		}

		m_table.DeleteNoDestructor();
		m_capacity = 0;
		m_size = 0;
	}

	template <typename K, typename V>
	void BaldLion::HashMap<K, V>::Delete()
	{
		for (ui32 i = 0; i < m_table.Size(); ++i)
		{
			HashMapNode<K, V>* nodeIterator = m_table[i];

			while (nodeIterator != nullptr)
			{
				HashMapNode<K, V>* temp = nodeIterator->nextNode;

				MemoryManager::Delete(nodeIterator);

				nodeIterator = temp;
			}
		}

		m_table.Delete(); 
		m_capacity = 0;
		m_size = 0;
	}

	template <typename K, typename V>
	ui32 BaldLion::HashMap<K, V>::IndexOf(const K& key) const
	{
		const hashType hashedMapKey = std::hash<K>()(key);
		const i32 tableIndex = FindIndex(hashedMapKey);

		BL_ASSERT(tableIndex >= 0, "Key not contained");

		return (ui32)tableIndex;
	}

	template <typename K, typename V>
	BaldLion::HashMap<K, V>& BaldLion::HashMap<K, V>::operator=(HashMap<K, V>&& other) noexcept
	{
		if (&other == this)
			return *this;

		Delete();

		m_allocationType = other.m_allocationType;
		m_size = other.m_size;
		m_capacity = other.m_capacity;
		m_table = std::move(other.m_table);		
		m_firstElementIndex = other.m_firstElementIndex;
		m_lastElementIndex = other.m_lastElementIndex;

		return *this;
	}

	template <typename K, typename V>
	BaldLion::HashMap<K, V>& BaldLion::HashMap<K, V>::operator=(const HashMap<K, V>& other)
	{
		if (&other == this)
			return *this;

		m_allocationType = other.m_allocationType;
		m_size = other.m_size;
		m_capacity = other.m_capacity;
		m_table = DynamicArray<HashMapNode<K, V>*>(other.m_table);
		m_firstElementIndex = other.m_firstElementIndex;
		m_lastElementIndex = other.m_lastElementIndex;

		return *this;
	}

	template <typename K, typename V>
	typename BaldLion::HashMap<K, V>::Iterator BaldLion::HashMap<K, V>::Begin()
	{
		return HashMap<K, V>::Iterator(this, m_firstElementIndex, m_table[m_firstElementIndex]);
	}

	template <typename K, typename V>
	typename BaldLion::HashMap<K, V> ::Iterator BaldLion::HashMap<K, V>::End()
	{
		return HashMap<K,V>::Iterator(this, m_lastElementIndex, nullptr);
	}

	template <typename K, typename V>
	void BaldLion::HashMap<K, V>::Reallocate(ui32 capacity)
	{
		DynamicArray<HashMapNode<K, V>*> newTable = DynamicArray<HashMapNode<K, V>*>(m_allocationType, capacity);		

		for (ui32 i = 0; i < m_table.Size(); ++i)
		{
			HashMapNode<K, V>* nodeIterator = m_table[i];
			
			while (nodeIterator != nullptr)
			{
				const hashType newIndex = nodeIterator->hashedMapKey % capacity;

				if (newTable[newIndex] == nullptr)
				{
					newTable[newIndex] = nodeIterator;
				}
				else {

					HashMapNode<K, V>* newTableIterator = newTable[newIndex];

					while (newTableIterator->nextNode != nullptr)
					{
						newTableIterator = newTableIterator->nextNode;
					}

					newTableIterator->nextNode = nodeIterator;
				}

				nodeIterator = nodeIterator->nextNode;
			}
		}

		m_table.Delete();
		m_table = std::move(newTable);
		m_capacity = capacity;

		newTable.Delete();
	}

	template <typename K, typename V>
	ui32 BaldLion::HashMap<K, V>::FindFirstElementIndex()
	{
		for (ui32 i = 0; i < m_table.Size(); ++i)
		{
			if (m_table[i] != nullptr)
			{
				return i;
			}
		}

		return m_capacity;
	}
}
