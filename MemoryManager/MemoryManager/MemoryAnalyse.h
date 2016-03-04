#pragma once

#include"MMutil.h"
#include"ConfigManager.h"

// 【对象热点分析】 【内存泄露分析】 【峰值记录】 【内存分配情况】等等

// 内存块信息
struct MemoryBlockInfo
{
	void*	_ptr;		// 指向内存块的指针
	string	_type;		// 类型
	int		_num;		// 数据个数
	size_t	_size;		// 数据块大小

	string	_filename;	// 文件名
	int		_fileline;	// 行号

	MemoryBlockInfo(void* ptr = 0, const char* type = "", int num = 0, size_t size = 0,
		const char* filename = "", int fileline = 0);

	// 保存数据到适配器。
	void Save(SaveAdapter& SA);
};

struct CountInfo
{
	int _addCount;		// 添加计数
	int _delCount;		// 删除计数

	long long _totalSize;	// 分配总大小

	long long _usedSize;	// 正在使用大小
	long long _maxUsedSize;	// 最大使用大小(最大峰值)

	CountInfo();
	void AddCount(size_t size);
	void DelCount(size_t size);
	void Save(SaveAdapter& SA);
};

class MemoryAnalyse : public Singleton<MemoryAnalyse>
{
	typedef unordered_map<void*, MemoryBlockInfo> MemoryLeakMap;
	typedef unordered_map<string, CountInfo> MemoryHostMap;
	typedef vector<MemoryBlockInfo> MemoryBlcokInfos;

public:
	// 更新剖析信息记录
	void AddRecord(const MemoryBlockInfo& memBlockInfo);
	void DelRecord(const MemoryBlockInfo& memBlockInfo);

	// 保存剖析信息相应的适配器
	void Save(SaveAdapter& SA);
	void OutPutLeakBlockInfo(SaveAdapter& SA);
	void OutPutHostObjectInfo(SaveAdapter& SA);
	void OutPutAllBlockInfo(SaveAdapter& SA);

	// 保存剖析信息
	static void Save();
	// 清理容器中保存的剖析数据
	void Clear();

private:
	MemoryAnalyse();
	~MemoryAnalyse();

	friend class Singleton<MemoryAnalyse>;

private:
	MemoryLeakMap	_leakBlockMap;			// 内存泄露块记录
	MemoryHostMap	_hostObjectMap;			// 热点对象(以对象类型为key进行统计)
	MemoryBlcokInfos _memBlcokInfos;		// 内存块分配记录

	CountInfo		_allocCountInfo;		// 分配计数记录
};