#pragma once

#include"MMUtil.h"
#include"MemoryAnalyse.h"

// 内存管理
class MemoryManager : public Singleton<MemoryManager>
{
public:
	// 申请内存并根据配置选项记录剖析数据
	void* Alloc(size_t size, const char* type,
		int num, const char* filename, int fileline);

	// 释放内存并根据剖析信息更新剖析数据
	void Dealloc(void* ptr, const char* type, size_t size);

private:
	MemoryManager();
	~MemoryManager();

	friend class Singleton<MemoryManager>;

private:
	allocator<char>	_allocPool;			// 内存池对象
};

template<typename T>
inline T* __ALLOC_TYPE(const char* filename, int fileline)
{
	return new (MemoryManager::GetInstance()->Alloc
		(sizeof(T), typeid(T).name(), 1, filename, fileline))T();
}

template<typename T>
inline void __DEALLOC_TYPE(T* ptr)
{
	if (ptr)
	{
		ptr->~T();
		MemoryManager::GetInstance()->Dealloc(ptr, typeid(T).name(), sizeof(T));
	}
}

template<typename T>
inline T* __ALLOC_TYPE_ARRAY(size_t num, const char* filename, int fileline)
{
	T* retPtr = 0;
	if (num <= 1)
	{
		retPtr = __ALLOC_TYPE<T>(filename, fileline);
	}
	else
	{
		//
		// 多申请四个字节存储元素的个数
		//
		void* ptr = (char*)MemoryManager::GetInstance()->Alloc\
			(sizeof(T)*num + 4, typeid(T).name(), num, filename, fileline);

		(int)(*(int*)ptr) = num;
		retPtr = (T*)((int)ptr + 4);

		//
		// 使用类型萃取判断该类型是否需要调用构造函数初始化
		//【未自定义构造函数析构函数的类型不需要调】
		//
		if (is_pod<T>::value == false)
		{
			for (size_t i = 0; i < num; ++i)
			{
				new(&retPtr[i])T();
			}
		}
	}

	return retPtr;
}

template<typename T>
inline void __DEALLOC_TYPE_ARRAY(T* ptr)
{
	T* selfPtr = (T*)((int)ptr - 4);
	int num = (int)(*(int*)(selfPtr));
	if (num <= 1)
	{
		__DEALLOC_TYPE(ptr);
	}
	else
	{
		//
		// 使用【类型萃取】判断该类型是否需要调用析构函数
		//【未自定义构造函数析构函数的类型不需要调】
		//
		if (is_pod<T>::value == false)
		{
			for (int i = 0; i < num; ++i)
			{
				ptr[i].~T();
			}
		}

		MemoryManager::GetInstance()->Dealloc(selfPtr, typeid(T).name(), sizeof(T)*num);
	}
}