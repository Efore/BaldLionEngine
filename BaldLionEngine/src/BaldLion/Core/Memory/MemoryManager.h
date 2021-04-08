#pragma once
#include "MemoryUtils.h"
#include "LinearAllocator.h"
#include "StackAllocator.h"
#include "FreeListAllocator.h"
#include "BaldLion/Core/StringId.h"
#include <unordered_map>
#include <mutex>

namespace BaldLion
{
	namespace Memory
	{
		enum class AllocationType {
			FreeList_Main,
			Linear_Frame,
			Stack,
			FreeList_Renderer,
		};

		struct AllocationInfo {

			StringId allocationDescription;
			AllocationType allocationType;
			size_t allocationSize;

			AllocationInfo(){}
			AllocationInfo (StringId description, AllocationType type, size_t size) : allocationDescription(description), allocationType(type), allocationSize(size){}
		};

		class MemoryManager {

		public:
			static void Init(size_t memoryAllocationSize); 
			static void Clear(AllocationType allocationType);
			static void Stop();			

			template <class T, class... Args >
			static T* New(StringId allocationName, AllocationType allocationType, Args&&... args);

			template <class T>
			static T* NewArray(StringId allocationName, AllocationType allocationType, ui32 size);

			template <class T>
			static void DeleteNoDestructor(T* element, bool isArray = false);
			template <class T>
			static void Delete(T* element, bool isArray = false);

			static Allocator* GetAllocator(AllocationType allocationType);

			static size_t GetAllocatorSize(AllocationType allocationType);
			static size_t GetAllocatorUsedMemory(AllocationType allocationType);

			template <class T>
			static AllocationType GetAllocatorType(const T* element);

			static size_t GetMemorySize();

		private:

			static FreeListAllocator* s_freeListMainAllocator;
			static FreeListAllocator* s_freeListRendererAllocator;
			static LinearAllocator* s_linearFrameAllocator;
			static StackAllocator* s_stackAllocator;

			static void* s_memory;

			static std::unordered_map<void*, AllocationInfo> s_allocationMap;

			static std::mutex s_mutex;
			static std::mutex s_arrayMutex;

			static size_t s_memorySize;
		};

		template <class T, class... Args >
		T* MemoryManager::New(StringId allocationName, AllocationType allocationType, Args&&... args)
		{
			const std::lock_guard<std::mutex> lock(s_mutex);

			if (s_memorySize == 0) Init(0);

			T* result = nullptr;

			switch (allocationType)
			{
			case AllocationType::FreeList_Main:
				result = new (s_freeListMainAllocator->Allocate(sizeof(T), __alignof(T))) T(std::forward<Args>(args)...);
				break;
			case AllocationType::Linear_Frame:
				result = new (s_linearFrameAllocator->Allocate(sizeof(T), __alignof(T))) T(std::forward<Args>(args)...);
				break;
			case AllocationType::Stack:
				result = new (s_stackAllocator->Allocate(sizeof(T), __alignof(T))) T(std::forward<Args>(args)...);
				break;
			case AllocationType::FreeList_Renderer:
				result = new (s_freeListRendererAllocator->Allocate(sizeof(T), __alignof(T))) T(std::forward<Args>(args)...);
				break;
			}		

			s_allocationMap.emplace((void*)result, AllocationInfo(allocationName, allocationType, sizeof(T)));
			
			return result;
		}

		template <class T>
		T* MemoryManager::NewArray(StringId allocationName, AllocationType allocationType, ui32 size)
		{
			const std::lock_guard<std::mutex> lock(s_arrayMutex);

			if (s_memorySize == 0) Init(0);

			T* result = nullptr;

			switch (allocationType)
			{
			case AllocationType::FreeList_Main:
				result = (T*)s_freeListMainAllocator->Allocate(size * sizeof(T), __alignof(T));
				break;
			case AllocationType::Linear_Frame:
				result = (T*)s_linearFrameAllocator->Allocate(size * sizeof(T), __alignof(T));;
				break;
			case AllocationType::Stack:
				result = (T*)s_stackAllocator->Allocate(size * sizeof(T), __alignof(T));;
				break;
			case AllocationType::FreeList_Renderer:
				result = (T*)s_freeListRendererAllocator->Allocate(size * sizeof(T), __alignof(T));;
				break;
			}

			s_allocationMap.emplace((void*)result, AllocationInfo(allocationName, allocationType, size * sizeof(T)));

			return result;
		}

		template <class T>
		void MemoryManager::DeleteNoDestructor(T* element,bool isArray)
		{
			std::lock_guard<std::mutex> lock(isArray ? s_arrayMutex : s_mutex);

			BL_ASSERT(element != nullptr, "element cannot be null");

			if (s_allocationMap.find(element) == s_allocationMap.end())
				return;

			AllocationType allocationType = s_allocationMap[element].allocationType;

			switch (allocationType)
			{
			case AllocationType::FreeList_Main:
				if(s_freeListMainAllocator != nullptr)
					s_freeListMainAllocator->Deallocate(element);
				break;
			case AllocationType::Linear_Frame:
				if (s_freeListMainAllocator != nullptr)
					s_freeListMainAllocator->Deallocate(element);
				break;
			case AllocationType::Stack:
				if (s_freeListMainAllocator != nullptr)
					s_freeListMainAllocator->Deallocate(element);
				break;
			case AllocationType::FreeList_Renderer:
				if (s_freeListMainAllocator != nullptr)
					s_freeListMainAllocator->Deallocate(element);
				break;
			}

			s_allocationMap.erase(element);
		}
		
		template <class T>
		void MemoryManager::Delete(T* element, bool isArray)
		{
			BL_ASSERT(element != nullptr, "element cannot be null");

			element->~T();
			DeleteNoDestructor(element, isArray);
		}

		template <class T>
		AllocationType MemoryManager::GetAllocatorType(const T* element)
		{
			if (s_allocationMap.find(element) != s_allocationMap.end())
				return s_allocationMap[element];

			return AllocationType::FreeList_Main;
		}

		//STL Allocators

		template<typename T>
		class STLFreeListMainAllocator
		{
		public:
			typedef T value_type;
			typedef T* pointer;
			typedef const T* const_pointer;
			typedef T& reference;
			typedef const T& const_reference;
			typedef std::size_t size_type;
			typedef std::ptrdiff_t difference_type;

			template<typename U>
			struct rebind { typedef STLFreeListMainAllocator<U> other; };

			pointer address(reference value) const { return &value; }
			const_pointer address(const_reference value) const { return &value; }

			STLFreeListMainAllocator() {}
			STLFreeListMainAllocator(const STLFreeListMainAllocator&) {}
			template<typename U>
			STLFreeListMainAllocator(const STLFreeListMainAllocator<U>&) {}
			~STLFreeListMainAllocator() {}

			size_type max_size() const { return SIZE_MAX / sizeof(T); }

			pointer allocate(size_type num, const void* = 0)
			{

				return MemoryManager::NewArray<T>(STRING_TO_ID("STL Allocator"),AllocationType::FreeList_Main, (ui32)num);
			}
			void construct(pointer p, const T& value)
			{
				new (p) T(value);
			}
			void destroy(pointer p)
			{
				p->~T();
			}
			void deallocate(pointer p, size_type)
			{
				MemoryManager::Delete(p);
			}
		};

		template<typename T1, typename T2>
		bool operator==(const STLFreeListMainAllocator<T1>&, const STLFreeListMainAllocator<T2>&) { return true; }
		template<typename T1, typename T2>
		bool operator!=(const STLFreeListMainAllocator<T1>&, const STLFreeListMainAllocator<T2>&) { return false; }

		//--------------------------------------------------------------------------

		template<typename T>
		class STLFreeListRendererAllocator
		{
		public:
			typedef T value_type;
			typedef T* pointer;
			typedef const T* const_pointer;
			typedef T& reference;
			typedef const T& const_reference;
			typedef std::size_t size_type;
			typedef std::ptrdiff_t difference_type;

			template<typename U>
			struct rebind { typedef STLFreeListRendererAllocator<U> other; };

			pointer address(reference value) const { return &value; }
			const_pointer address(const_reference value) const { return &value; }

			STLFreeListRendererAllocator() {}
			STLFreeListRendererAllocator(const STLFreeListRendererAllocator&) {}
			template<typename U>
			STLFreeListRendererAllocator(const STLFreeListRendererAllocator<U>&) {}
			~STLFreeListRendererAllocator() {}

			size_type max_size() const { return SIZE_MAX / sizeof(T); }

			pointer allocate(size_type num, const void* = 0)
			{
				return MemoryManager::NewArray<T>(STRING_TO_ID("STL Allocator"), AllocationType::FreeList_Renderer, (ui32)num);
			}
			void construct(pointer p, const T& value)
			{
				new (p) T(value);
			}
			void destroy(pointer p)
			{
				p->~T();
			}
			void deallocate(pointer p, size_type)
			{
				MemoryManager::Delete(p);
			}
		};

		template<typename T1, typename T2>
		bool operator==(const STLFreeListRendererAllocator<T1>&, const STLFreeListRendererAllocator<T2>&) { return true; }
		template<typename T1, typename T2>
		bool operator!=(const STLFreeListRendererAllocator<T1>&, const STLFreeListRendererAllocator<T2>&) { return false; }

		//--------------------------------------------------------------------------

		template<typename T>
		class STLLinearFrameAllocator
		{
		public:
			typedef T value_type;
			typedef T* pointer;
			typedef const T* const_pointer;
			typedef T& reference;
			typedef const T& const_reference;
			typedef std::size_t size_type;
			typedef std::ptrdiff_t difference_type;

			template<typename U>
			struct rebind { typedef STLLinearFrameAllocator<U> other; };

			pointer address(reference value) const { return &value; }
			const_pointer address(const_reference value) const { return &value; }

			STLLinearFrameAllocator() {}
			STLLinearFrameAllocator(const STLLinearFrameAllocator&) {}
			template<typename U>
			STLLinearFrameAllocator(const STLLinearFrameAllocator<U>&) {}
			~STLLinearFrameAllocator() {}

			size_type max_size() const { return SIZE_MAX / sizeof(T); }

			pointer allocate(size_type num, const void* = 0)
			{
				return MemoryManager::NewArray<T>(STRING_TO_ID("STL Allocator"), AllocationType::Linear_Frame, (ui32)num);
			}
			void construct(pointer p, const T& value)
			{
				new (p) T(value);
			}
			void destroy(pointer p)
			{
				p->~T();
			}
			void deallocate(pointer p, size_type)
			{
				MemoryManager::Delete(p);
			}
		};

		template<typename T1, typename T2>
		bool operator==(const STLLinearFrameAllocator<T1>&, const STLLinearFrameAllocator<T2>&) { return true; }
		template<typename T1, typename T2>
		bool operator!=(const STLLinearFrameAllocator<T1>&, const STLLinearFrameAllocator<T2>&) { return false; }

		//--------------------------------------------------------------------------

		template<typename T>
		class STLStackAllocator
		{
		public:
			typedef T value_type;
			typedef T* pointer;
			typedef const T* const_pointer;
			typedef T& reference;
			typedef const T& const_reference;
			typedef std::size_t size_type;
			typedef std::ptrdiff_t difference_type;

			template<typename U>
			struct rebind { typedef STLStackAllocator<U> other; };

			pointer address(reference value) const { return &value; }
			const_pointer address(const_reference value) const { return &value; }

			STLStackAllocator() {}
			STLStackAllocator(const STLStackAllocator&) {}
			template<typename U>
			STLStackAllocator(const STLStackAllocator<U>&) {}
			~STLStackAllocator() {}

			size_type max_size() const { return SIZE_MAX / sizeof(T); }

			pointer allocate(size_type num, const void* = 0)
			{
				return MemoryManager::NewArray<T>(STRING_TO_ID("STL Allocator"), AllocationType::Stack, (ui32)num);
			}
			void construct(pointer p, const T& value)
			{
				new (p) T(value);
			}
			void destroy(pointer p)
			{
				p->~T();
			}
			void deallocate(pointer p, size_type)
			{
				MemoryManager::Delete(p);
			}
		};

		template<typename T1, typename T2>
		bool operator==(const STLStackAllocator<T1>&, const STLStackAllocator<T2>&) { return true; }
		template<typename T1, typename T2>
		bool operator!=(const STLStackAllocator<T1>&, const STLStackAllocator<T2>&) { return false; }
	}
}