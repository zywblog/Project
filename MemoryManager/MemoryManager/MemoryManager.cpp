#include"MemoryManager.h"
#include"IPCManager.h"

MemoryManager::MemoryManager()
{
	// ��ʹ���˶��õ��ڴ����ӿڣ�������IPC�����߼����̡߳�
	IPCOnlineServer::GetInstance()->Start();
}

MemoryManager::~MemoryManager()
{
}

void* MemoryManager::Alloc(size_t size, const char* type,
	int num, const char* filename, int fileline)
{
	// �滻Ϊ�ڴ�� or ��Чmalloc
	//void* ptr = malloc(size);
	void* ptr = _allocPool.allocate(size);

	if (ConfigManager::GetInstance()->GetOptions() != CO_NONE)
	{
		size_t realSize = num > 1 ? size - 4 : size;
		MemoryBlockInfo info(ptr, type, num, realSize, filename, fileline);
		MemoryAnalyse::GetInstance()->AddRecord(info);
	}

	return ptr;
}

void MemoryManager::Dealloc(void* ptr, const char* type, size_t size)
{
	if (ptr)
	{
		// �滻Ϊ�ڴ�� or ��Чfree
		//free(ptr);
		_allocPool.deallocate((char*)ptr, size);

		if (ConfigManager::GetInstance()->GetOptions() != CO_NONE)
		{
			MemoryBlockInfo info(ptr, type, 0, size, "", 0);
			MemoryAnalyse::GetInstance()->DelRecord(info);
		}
	}
}