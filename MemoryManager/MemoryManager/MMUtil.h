#pragma once
#include"CommonHeader.h"

// ��Ϣ����������
class SaveAdapter
{
public:
	virtual int Save(char* format, ...) = 0;
};

// ����̨����������
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

// �ļ�����������
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

// ������
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
		// ������ֹ�ڶ��̻߳��������ɶ������ʵ����
		//
		if (_sInstance == 0)
		{
			LockGuard<mutex> lock(_sMutex);
			if (_sInstance == 0)
			{
				// ���Ż���
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
	static T* _sInstance;	// ��ʵ������
	static mutex _sMutex;	// ���̻߳�����
};

template<class T>
T* Singleton<T>::_sInstance = 0;

template<class T>
mutex Singleton<T>::_sMutex;