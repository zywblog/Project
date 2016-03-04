#pragma once
#include"CommonHeader.h"

// 信息保存适配器
class SaveAdapter
{
public:
	virtual int Save(char* format, ...) = 0;
};

// 控制台保存适配器
class ConsoleSaveAdapter : public SaveAdapter
{
public:
	virtual int Save(char* format, ...)
	{
		va_list argPtr;
		int cnt;

		va_start(argPtr, format);
		cnt = vfprintf(stdout, format, argPtr);
		va_end(argPtr);

		return cnt;
	}
};

// 文件保存适配器
class FileSaveAdapter : public SaveAdapter
{
public:
	FileSaveAdapter(const char* path)
		:_fOut(0)
	{
		_fOut = fopen(path, "w");
	}

	~FileSaveAdapter()
	{
		if (_fOut)
		{
			fclose(_fOut);
		}
	}

	virtual int Save(char* format, ...)
	{
		if (_fOut)
		{
			va_list argPtr;
			int cnt;

			va_start(argPtr, format);
			cnt = vfprintf(_fOut, format, argPtr);
			va_end(argPtr);

			return cnt;
		}

		return 0;
	}
private:
	FileSaveAdapter(const FileSaveAdapter&);
	FileSaveAdapter& operator==(const FileSaveAdapter&);

private:
	FILE* _fOut;
};

// 锁守卫
template<class T>
class LockGuard
{
public:
	LockGuard(T& x)
		:_mutex(x)
	{
		_mutex.lock();
	}

	~LockGuard()
	{
		_mutex.unlock();
	}

private:
	LockGuard(const LockGuard&);
	LockGuard& operator=(const LockGuard&);
private:
	T& _mutex;
};

template<class T>
class Singleton
{
public:
	static T* GetInstance()
	{
		//
		// 加锁防止在多线程环境下生成多个对象实例。
		//
		if (_sInstance == 0)
		{
			LockGuard<mutex> lock(_sMutex);
			if (_sInstance == 0)
			{
				// 需优化？
#ifdef _WIN32 
				MemoryBarrier();
#endif
				_sInstance = new T();
			}
		}
		return _sInstance;
	}

protected:
	Singleton()
	{}
	virtual ~Singleton()
	{}

protected:
	static T* _sInstance;	// 单实例对象
	static mutex _sMutex;	// 多线程互斥锁
};

template<class T>
T* Singleton<T>::_sInstance = 0;

template<class T>
mutex Singleton<T>::_sMutex;