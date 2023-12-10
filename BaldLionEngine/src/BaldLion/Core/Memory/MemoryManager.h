#pragma once
#include "MemoryUtils.h"
#include "LinearAllocator.h"
#include "StackAllocator.h"
#include "FreeListAllocator.h"
#include "BaldLion/Core/StringId.h"
#include "BaldLion/Core/Profiling.h"
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
			FreeList_ECS,
			FreeList_Resources,
			FreeList_PhysX
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
			static void Delete(AllocationType allocationType);
			static void Stop();			

			template <class T, class... Args >
			static T* New(const char* allocationName, AllocationType allocationType, Args&&... args);

			template <class T>
			static T* NewArray(const char* allocationName, AllocationType allocationType, ui32 size);

			template <class T>
			static void DeleteNoDestructor(T* element);
			template <class T>
			static void Delete(T* element);

			static Allocator* GetAllocator(AllocationType allocationType);

			static size_t GetAllocatorSize(AllocationType allocationType);
			static size_t GetAllocatorUsedMemory(AllocationType allocationType);

			template <class T>
			static AllocationType GetAllocatorType(const T* element);

			static size_t GetMemorySize();

		private:

			static FreeListAllocator* s_freeListMainAllocator;
			static FreeListAllocator* s_freeListRendererAllocator;
			static FreeListAllocator* s_freeListECSAllocator;
			static FreeListAllocator* s_freeListResourcesAllocator;
			static FreeListAllocator* s_freeListPhysXAllocator;

			static LinearAllocator* s_linearFrameAllocator;
			static StackAllocator* s_stackAllocator;

			static void* s_memory;

			static std::unordered_map<void*, AllocationInfo> s_allocationMap;

			static std::mutex s_mutex;

			static size_t s_memorySize;
		};

		template <class T, class... Args >
		T* MemoryManager::New(const char* allocationName, AllocationType allocationType, Args&&... args)
		{
			BL_DEEP_PROFILE_FUNCTION();

			void* allocation = nullptr;
			T* result = nullptr;

			{
				if (s_memorySize == 0) Init(0);

				switch (allocationType)
				{
				case AllocationType::FreeList_Main:
					allocation = s_freeListMainAllocator->Allocate(sizeof(T), __alignof(T));
					break;
				case AllocationType::Linear_Frame:
					allocation = s_linearFrameAllocator->Allocate(sizeof(T), __alignof(T));
					break;
				case AllocationType::Stack:
					allocation = s_stackAllocator->Allocate(sizeof(T), __alignof(T));
					break;
				case AllocationType::FreeList_Renderer:
					allocation = s_freeListRendererAllocator->Allocate(sizeof(T), __alignof(T));
					break;
				case AllocationType::FreeList_ECS:
					allocation = s_freeListECSAllocator->Allocate(sizeof(T), __alignof(T));
					break;
				case AllocationType::FreeList_Resources:
					allocation = s_freeListResourcesAllocator->Allocate(sizeof(T), __alignof(T));
					break;
				}

				if (allocation != nullptr) {
					const std::lock_guard<std::mutex> lock(s_mutex);
					s_allocationMap.emplace((void*)allocation, AllocationInfo(BL_STRING_TO_STRINGID(allocationName), allocationType, sizeof(T)));
				}
			}

			if (allocation != nullptr)
			{
				result = new (allocation) T(std::forward<Args>(args)...);
			}
			
			return result;
		}

		template <class T>
		T* MemoryManager::NewArray(const char* allocationName, AllocationType allocationType, ui32 size)
		{
			BL_DEEP_PROFILE_FUNCTION();

			T* result = nullptr;

			{
				BL_ASSERT(s_memorySize > 0, "Memory Manager not init");

				switch (allocationType)
				{
				case AllocationType::FreeList_Main:
					result = (T*)s_freeListMainAllocator->Allocate(size * sizeof(T), __alignof(T));
					break;
				case AllocationType::Linear_Frame:
					result = (T*)s_linearFrameAllocator->Allocate(size * sizeof(T), __alignof(T));
					break;
				case AllocationType::Stack:
					result = (T*)s_stackAllocator->Allocate(size * sizeof(T), __alignof(T));
					break;
				case AllocationType::FreeList_Renderer:
					result = (T*)s_freeListRendererAllocator->Allocate(size * sizeof(T), __alignof(T));
					break;
				case AllocationType::FreeList_ECS:
					result = (T*)s_freeListECSAllocator->Allocate(size * sizeof(T), __alignof(T));
					break;
				case AllocationType::FreeList_Resources:
					result = (T*)s_freeListResourcesAllocator->Allocate(size * sizeof(T), __alignof(T));
					break;
				}

				if (result != nullptr)
				{
					const std::lock_guard<std::mutex> lock(s_mutex);
					s_allocationMap.emplace((void*)result, AllocationInfo(BL_STRING_TO_STRINGID(allocationName), allocationType, size * sizeof(T)));
				}
			}

			return result;
		}

		template <class T>
		void MemoryManager::DeleteNoDestructor(T* element)
		{
			BL_ASSERT(element != nullptr, "element cannot be null");

			auto it = s_allocationMap.find(element);
			if (it == s_allocationMap.end())
				return;			

			AllocationType allocationType = s_allocationMap[element].allocationType;
			size_t remainingMemory = 0;

			switch (allocationType)
			{
			case AllocationType::FreeList_Main:
				if (s_freeListMainAllocator != nullptr)
				{
					s_freeListMainAllocator->Deallocate(element);
				}
				break;
			case AllocationType::Linear_Frame:
				if (s_linearFrameAllocator != nullptr)
				{
					s_linearFrameAllocator->Deallocate(element);
				}
				break;
			case AllocationType::Stack:
				if (s_stackAllocator != nullptr)
				{
					s_stackAllocator->Deallocate(element);
				}
				break;
			case AllocationType::FreeList_Renderer:
				if (s_freeListRendererAllocator != nullptr)
				{
					s_freeListRendererAllocator->Deallocate(element);
				}
				break;
			case AllocationType::FreeList_ECS:
				if (s_freeListECSAllocator != nullptr)
				{
					s_freeListECSAllocator->Deallocate(element);			
				}
				break;
			case AllocationType::FreeList_Resources:
				if (s_freeListResourcesAllocator != nullptr)
				{
					s_freeListResourcesAllocator->Deallocate(element);
				}
				break;
			case AllocationType::FreeList_PhysX:
				if (s_freeListPhysXAllocator != nullptr)
				{
					s_freeListPhysXAllocator->Deallocate(element);
				}
				break;
			}

			{
				const std::lock_guard<std::mutex> lock(s_mutex);
				s_allocationMap.erase(element);
			}
		}
		
		template <class T>
		void MemoryManager::Delete(T* element)
		{
			BL_ASSERT(element != nullptr, "element cannot be null");

			element->~T();
			DeleteNoDestructor(element);
		}

		template <class T>
		AllocationType MemoryManager::GetAllocatorType(const T* element)
		{
			if (s_allocationMap.find((void*)element) != s_allocationMap.end())
				return s_allocationMap[(void*)element].allocationType;

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

				return MemoryManager::NewArray<T>("STL Allocator",AllocationType::FreeList_Main, (ui32)num);
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
				return MemoryManager::NewArray<T>("STL Allocator", AllocationType::FreeList_Renderer, (ui32)num);
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
		class STLFreeListECSAllocator
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
			struct rebind { typedef STLFreeListECSAllocator<U> other; };

			pointer address(reference value) const { return &value; }
			const_pointer address(const_reference value) const { return &value; }

			STLFreeListECSAllocator() {}
			STLFreeListECSAllocator(const STLFreeListECSAllocator&) {}
			template<typename U>
			STLFreeListECSAllocator(const STLFreeListECSAllocator<U>&) {}
			~STLFreeListECSAllocator() {}

			size_type max_size() const { return SIZE_MAX / sizeof(T); }

			pointer allocate(size_type num, const void* = 0)
			{
				return MemoryManager::NewArray<T>("STL Allocator", AllocationType::FreeList_ECS, (ui32)num);
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

		//template<typename T1, typename T2>
		//bool operator==(const STLFreeListECSAllocator<T1>&, const STLFreeListECSAllocator<T2>&) { return true; }
		//template<typename T1, typename T2>
		//bool operator!=(const STLFreeListECSAllocator<T1>&, const STLFreeListECSAllocator<T2>&) { return false; }

		////--------------------------------------------------------------------------

		//template<typename T>
		//class STLLinearFrameAllocator
		//{
		//public:
		//	typedef T value_type;
		//	typedef T* pointer;
		//	typedef const T* const_pointer;
		//	typedef T& reference;
		//	typedef const T& const_reference;
		//	typedef std::size_t size_type;
		//	typedef std::ptrdiff_t difference_type;

		//	template<typename U>
		//	struct rebind { typedef STLLinearFrameAllocator<U> other; };

		//	pointer address(reference value) const { return &value; }
		//	const_pointer address(const_reference value) const { return &value; }

		//	STLLinearFrameAllocator() {}
		//	STLLinearFrameAllocator(const STLLinearFrameAllocator&) {}
		//	template<typename U>
		//	STLLinearFrameAllocator(const STLLinearFrameAllocator<U>&) {}
		//	~STLLinearFrameAllocator() {}

		//	size_type max_size() const { return SIZE_MAX / sizeof(T); }

		//	pointer allocate(size_type num, const void* = 0)
		//	{
		//		return MemoryManager::NewArray<T>("STL Allocator", AllocationType::Linear_Frame, (ui32)num);
		//	}
		//	void construct(pointer p, const T& value)
		//	{
		//		new (p) T(value);
		//	}
		//	void destroy(pointer p)
		//	{
		//		p->~T();
		//	}
		//	void deallocate(pointer p, size_type)
		//	{
		//		MemoryManager::Delete(p);
		//	}
		//};

		//template<typename T1, typename T2>
		//bool operator==(const STLLinearFrameAllocator<T1>&, const STLLinearFrameAllocator<T2>&) { return true; }
		//template<typename T1, typename T2>
		//bool operator!=(const STLLinearFrameAllocator<T1>&, const STLLinearFrameAllocator<T2>&) { return false; }

		////--------------------------------------------------------------------------

		//template<typename T>
		//class STLStackAllocator
		//{
		//public:
		//	typedef T value_type;
		//	typedef T* pointer;
		//	typedef const T* const_pointer;
		//	typedef T& reference;
		//	typedef const T& const_reference;
		//	typedef std::size_t size_type;
		//	typedef std::ptrdiff_t difference_type;

		//	template<typename U>
		//	struct rebind { typedef STLStackAllocator<U> other; };

		//	pointer address(reference value) const { return &value; }
		//	const_pointer address(const_reference value) const { return &value; }

		//	STLStackAllocator() {}
		//	STLStackAllocator(const STLStackAllocator&) {}
		//	template<typename U>
		//	STLStackAllocator(const STLStackAllocator<U>&) {}
		//	~STLStackAllocator() {}

		//	size_type max_size() const { return SIZE_MAX / sizeof(T); }

		//	pointer allocate(size_type num, const void* = 0)
		//	{
		//		return MemoryManager::NewArray<T>("STL Allocator", AllocationType::Stack, (ui32)num);
		//	}
		//	void construct(pointer p, const T& value)
		//	{
		//		new (p) T(value);
		//	}
		//	void destroy(pointer p)
		//	{
		//		p->~T();
		//	}
		//	void deallocate(pointer p, size_type)
		//	{
		//		MemoryManager::Delete(p);
		//	}
		//};

		//template<typename T1, typename T2>
		//bool operator==(const STLStackAllocator<T1>&, const STLStackAllocator<T2>&) { return true; }
		//template<typename T1, typename T2>
		//bool operator!=(const STLStackAllocator<T1>&, const STLStackAllocator<T2>&) { return false; }
	}

}
