#pragma once

#include"MMUtil.h"
#include"MemoryAnalyse.h"

// �ڴ����
class MemoryManager : public Singleton<MemoryManager>
{
public:
	// �����ڴ沢��������ѡ���¼��������
	void* Alloc(size_t size, const char* type,
		int num, const char* filename, int fileline);

	// �ͷ��ڴ沢����������Ϣ������������
	void Dealloc(void* ptr, const char* type, size_t size);

private:
	MemoryManager();
	~MemoryManager();

	friend class Singleton<MemoryManager>;

private:
	allocator<char>	_allocPool;			// �ڴ�ض���
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
		// �������ĸ��ֽڴ洢Ԫ�صĸ���
		//
		void* ptr = (char*)MemoryManager::GetInstance()->Alloc\
			(sizeof(T)*num + 4, typeid(T).name(), num, filename, fileline);

		(int)(*(int*)ptr) = num;
		retPtr = (T*)((int)ptr + 4);

		//
		// ʹ��������ȡ�жϸ������Ƿ���Ҫ���ù��캯����ʼ��
		//��δ�Զ��幹�캯���������������Ͳ���Ҫ����
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
		// ʹ�á�������ȡ���жϸ������Ƿ���Ҫ������������
		//��δ�Զ��幹�캯���������������Ͳ���Ҫ����
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