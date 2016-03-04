#include"MemoryManager.h"
#include"IPCManager.h"

MemoryManager::MemoryManager()
{
	// 若使用了定置的内存管理接口，则启动IPC的在线监听线程。
	IPCOnlineServer::GetInstance()->Start();
}

MemoryManager::~MemoryManager()
{
}

void* MemoryManager::Alloc(size_t size, const char* type,
	int num, const char* filename, int fileline)
{
	// 替换为内存池 or 高效malloc
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
		// 替换为内存池 or 高效free
		//free(ptr);
		_allocPool.deallocate((char*)ptr, size);

		if (ConfigManager::GetInstance()->GetOptions() != CO_NONE)
		{
			MemoryBlockInfo info(ptr, type, 0, size, "", 0);
			MemoryAnalyse::GetInstance()->DelRecord(info);
		}
	}
}