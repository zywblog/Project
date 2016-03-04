#include"MemoryManagerPrec.h"
#include"IPCManager.h"

class Array
{
public:
	Array(size_t size = 10)
		:_size(size)
		, _a(0)
	{
		//cout<<"Array(size_t size)"<<endl;

		if (_size > 0)
		{
			_a = new int[size];//����new ���÷�
		}
	}

	~Array()
	{
		//cout<<"~Array()"<<endl;

		if (_a)
		{
			delete[] _a;
			_a = 0;
			_size = 0;
		}
	}

	int& operator[](size_t size)
	{
		assert(size < _size);

		return _a[size];
	}
private:
	int*	_a;
	size_t	_size;
};

#include<iostream>

// ������������ & �Զ�������
void TestAllocDealloc()
{
	SET_MMA_OPTIONS(CO_ANALYSE_MEMORY_LEAK
		| CO_ANALYSE_MEMORY_HOST
		| CO_ANALYSE_MEMORY_HOST
		| CO_SAVE_TO_CONSOLE);

	// �����Զ�������

	Array* a1 = ALLOC_TYPE(Array);
	//DEALLOC_TYPE(a1);

	//Array* a2 = ALLOC_TYPE_ARRAY(Array, 3);
	//DEALLOC_TYPE_ARRAY(a2);

	//
	// ������������
	//
	int* p1 = ALLOC_TYPE(int);
	DEALLOC_TYPE(p1);
	int* p2 = ALLOC_TYPE_ARRAY(int, 3);
	DEALLOC_TYPE_ARRAY(p2);

	//MMA_SAVE();
}

// �����̰߳�ȫ����
void Alloc1()
{
	while (1)
	{
		cout << "Alloc1" << endl;

		int* p1 = ALLOC_TYPE(int);

		this_thread::sleep_for(chrono::milliseconds(50));

		DEALLOC_TYPE(p1);

		Array* a2 = ALLOC_TYPE_ARRAY(Array, 10);

		this_thread::sleep_for(chrono::milliseconds(30));

		DEALLOC_TYPE_ARRAY(a2);
	}
}

void Alloc2()
{
	while (1)
	{
		cout << "Alloc2" << endl;

		Array* a2 = ALLOC_TYPE_ARRAY(Array, 10);

		this_thread::sleep_for(chrono::milliseconds(40));

		DEALLOC_TYPE_ARRAY(a2);

		int* p1 = ALLOC_TYPE(int);

		this_thread::sleep_for(chrono::milliseconds(60));

		DEALLOC_TYPE(p1);
	}
}

void TestMultiThread()//���߳�test 
{
	SET_MMA_OPTIONS(CO_ANALYSE_MEMORY_LEAK
		| CO_ANALYSE_MEMORY_HOST
		| CO_ANALYSE_MEMORY_HOST
		| CO_SAVE_TO_CONSOLE);


	thread t1(Alloc1);
	thread t2(Alloc2);
	thread t3(Alloc1);
	thread t4(Alloc1);


	t1.join();
	t2.join();
	t3.join();
	t4.join();
}

// ����IPC�ܵ�����
void TestIPC()
{
	int count = 10;
	//IPCOnlineServer::GetInstance()->Start();

	while (1)
	{
		int* p1 = ALLOC_TYPE(int);

		this_thread::sleep_for(chrono::milliseconds(3000));

		//DEALLOC_TYPE(p1);

		if (count--)
		{
			DEALLOC_TYPE(p1);
		}
		else
		{
			count = 10;
		}

		this_thread::sleep_for(chrono::milliseconds(3000));
	}
}
