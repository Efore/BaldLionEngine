#pragma once
#include "BaldLion/Core/Memory/MemoryManager.h"

namespace BaldLion
{
	template <typename K, typename V, typename Allocator = Memory::Allocator>
	class HashMap
	{
		public:

			HashMap();
			HashMap(Memory::AllocationType allocationType, size_t capacity);

			bool Contains(const K& key) const;
			
			V& operator[](const K& key);
			const V& operator[](const K& key) const;

			void Insert(const K& key, const V& value);
			void Insert(K&& key, V&& value);

			bool Remove(const K& key);

			void Clear();
			void ClearNoDestructor();

			
		public:

			struct Iterator {
				size_t index;
				K key;
				V* value;
			};

			Iterator Begin();
			Iterator Next(const Iterator& it);
			Iterator End();

		private:

			void Rehash(size_t size);
			void Reserve(size_t capacity);

		private:

			Allocator* m_allocator = nullptr;		
			size_t m_capacity = 0;
	
	};
}
