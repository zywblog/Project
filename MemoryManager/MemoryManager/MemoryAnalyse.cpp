#include"MemoryAnalyse.h"

#define BEGIN_LINE		\
	("-----------------------------------------------------------------\r\n")

#define END_LINE		\
	("-----------------------------------------------------------------\r\n")


//获取路径中最后的文件名。
string GetFileName(const string& path)
{
	char ch = '/';

#ifdef _WIN32
	ch = '\\';
#endif

	size_t pos = path.rfind(ch);
	if (pos == string::npos)
	{
		return path;
	}
	else
	{
		return path.substr(pos + 1);
	}
}

///////////////////////////////////////////////////////////////////////////////////
// MemoryBlockInfo

MemoryBlockInfo::MemoryBlockInfo(void* ptr,
	const char* type,
	int num,
	size_t size,
	const char* filename,
	int fileline)
	: _ptr(ptr)
	, _type(type)
	, _num(num)
	, _size(size)
	, _filename(GetFileName(filename))
	, _fileline(fileline)
{}

void MemoryBlockInfo::Save(SaveAdapter& SA)
{
	SA.Save("Blcok->Ptr:0X%x , Type:%s , Num:%d, Size:%d\r\nFilename:%s , Fileline:%d\r\n",
		_ptr, _type.c_str(), _num, _size, _filename.c_str(), _fileline);
}

////////////////////////////////////////////////////////////////////////
//CountInfo

CountInfo::CountInfo()
: _addCount(0)
, _delCount(0)
, _totalSize(0)
, _usedSize(0)
, _maxUsedSize(0)
{}

void CountInfo::AddCount(size_t size)
{
	_addCount++;
	_totalSize += size;
	_usedSize += size;

	if (_usedSize > _maxUsedSize)
	{
		_maxUsedSize = _usedSize;
	}
}

void CountInfo::DelCount(size_t size)
{
	_delCount++;
	_usedSize -= size;
}

void CountInfo::Save(SaveAdapter& SA)
{
	SA.Save("Alloc Count:%d , Dealloc Count:%d, Alloc Total Size:%lld byte, NonRelease Size:%lld, Max Used Size:%lld\r\n",
		_addCount, _delCount, _totalSize, _usedSize, _maxUsedSize);
}

/////////////////////////////////////////////////////////////////////////
//MemoryAnalyse

MemoryAnalyse::MemoryAnalyse()
{}

MemoryAnalyse::~MemoryAnalyse()
{}

void MemoryAnalyse::AddRecord(const MemoryBlockInfo& memBlockInfo)
{
	int flag = ConfigManager::GetInstance()->GetOptions();

	void* ptr = memBlockInfo._ptr;
	const string& type = memBlockInfo._type;
	const size_t& size = memBlockInfo._size;

	LockGuard<mutex> lock(_sMutex);

	// 更新内存泄露块记录
	if (flag & CO_ANALYSE_MEMORY_LEAK)
	{
		_leakBlockMap[ptr] = memBlockInfo;
	}

	// 更新内存热点记录
	if (flag & CO_ANALYSE_MEMORY_HOST)
	{
		MemoryHostMap::iterator it = _hostObjectMap.find(type);
		if (it != _hostObjectMap.end())
		{
			it->second.AddCount(size);
		}
		else
		{
			CountInfo cntInfo;
			cntInfo.AddCount(size);
			_hostObjectMap[type] = cntInfo;
		}
	}
	// 更新内存块分配信息(内存块记录&分配计数)
	if (flag & CO_ANALYSE_MEMORY_ALLOC_INFO)
	{
		_memBlcokInfos.push_back(memBlockInfo);

		_allocCountInfo.AddCount(size);
	}
}

void MemoryAnalyse::DelRecord(const MemoryBlockInfo& memBlockInfo)
{
	int flag = ConfigManager::GetInstance()->GetOptions();

	void* ptr = memBlockInfo._ptr;
	const string& type = memBlockInfo._type;
	const size_t& size = memBlockInfo._size;

	LockGuard<mutex> lock(_sMutex);

	// 更新内存泄露块记录
	if (flag & CO_ANALYSE_MEMORY_LEAK)
	{
		MemoryLeakMap::iterator it = _leakBlockMap.find(ptr);
		if (it != _leakBlockMap.end())
		{
			_leakBlockMap.erase(it);
		}
		else
		{
			perror("【Leak】Record Memory Block No Match");
		}
	}

	// 更新内存热点记录
	if (flag & CO_ANALYSE_MEMORY_HOST)
	{
		MemoryHostMap::iterator it = _hostObjectMap.find(type);
		if (it != _hostObjectMap.end())
		{
			it->second.DelCount(size);
		}
		else
		{
			perror("【host】Record Memory Block No Match");
		}
	}

	// 更新内存块分配计数
	if (flag & CO_ANALYSE_MEMORY_ALLOC_INFO)
	{
		_allocCountInfo.DelCount(memBlockInfo._size);
	}
}

void MemoryAnalyse::Save()
{
	int flag = ConfigManager::GetInstance()->GetOptions();

	if (flag & CO_SAVE_TO_CONSOLE)
	{
		MemoryAnalyse::GetInstance()->Save(ConsoleSaveAdapter());
	}

	if (flag & CO_SAVE_TO_FILE)
	{
		const string& path = ConfigManager::GetInstance()->GetOutputPath();
		MemoryAnalyse::GetInstance()->Save(FileSaveAdapter(path.c_str()));
	}
}

void MemoryAnalyse::Save(SaveAdapter& SA)
{
	int flag = ConfigManager::GetInstance()->GetOptions();

	SA.Save("---------------------------【Analyse Report】---------------------------\r\n");

	if (flag & CO_ANALYSE_MEMORY_LEAK)
	{
		OutPutLeakBlockInfo(SA);
	}
	if (flag & CO_ANALYSE_MEMORY_HOST)
	{
		OutPutHostObjectInfo(SA);
	}
	if (flag & CO_ANALYSE_MEMORY_ALLOC_INFO)
	{
		OutPutAllBlockInfo(SA);
	}

	SA.Save("-----------------------------【   end   】----------------------------\r\n");
}

void MemoryAnalyse::Clear()
{
	LockGuard<mutex> lock(_sMutex);

	_leakBlockMap.clear();
	_hostObjectMap.clear();
	_memBlcokInfos.clear();
}

void MemoryAnalyse::OutPutLeakBlockInfo(SaveAdapter& SA)
{
	//SA.Save(BEGIN_LINE);

	{
		LockGuard<mutex> lock(_sMutex);

		if (!_leakBlockMap.empty())
		{
			SA.Save("\r\n【Memory Leak Block Info】\r\n");
		}

		int index = 1;
		MemoryLeakMap::iterator it = _leakBlockMap.begin();
		for (; it != _leakBlockMap.end(); ++it)
		{
			SA.Save("NO%d.\r\n", index++);
			it->second.Save(SA);
		}
	}

	//SA.Save(BEGIN_LINE);
}
void MemoryAnalyse::OutPutHostObjectInfo(SaveAdapter& SA)
{
	//SA.Save(BEGIN_LINE);

	{
		LockGuard<mutex> lock(_sMutex);

		if (!_hostObjectMap.empty())
		{
			SA.Save("\r\n【Memory Host Object Statistics】\r\n");
		}

		int index = 1;
		MemoryHostMap::iterator it = _hostObjectMap.begin();
		for (; it != _hostObjectMap.end(); ++it)
		{
			SA.Save("NO%d.\r\n", index++);
			SA.Save("type:%s , ", it->first.c_str());
			it->second.Save(SA);
		}
	}

	//SA.Save(BEGIN_LINE);
}

void MemoryAnalyse::OutPutAllBlockInfo(SaveAdapter& SA)
{
	if (_memBlcokInfos.empty())
	{
		return;
	}

	//SA.Save(BEGIN_LINE);

	{
		LockGuard<mutex> lock(_sMutex);

		SA.Save("\r\n【Alloc Count Statistics】\r\n");
		_allocCountInfo.Save(SA);

		SA.Save("\r\n【All Memory Blcok Info】\r\n");
		int index = 1;
		MemoryBlcokInfos::iterator it = _memBlcokInfos.begin();
		for (; it != _memBlcokInfos.end(); ++it)
		{
			SA.Save("NO%d.\r\n", index++);
			it->Save(SA);
		}
	}

	//SA.Save(BEGIN_LINE);
}

