#pragma once

#include"MMutil.h"
#include"ConfigManager.h"

// �������ȵ������ ���ڴ�й¶������ ����ֵ��¼�� ���ڴ����������ȵ�

// �ڴ����Ϣ
struct MemoryBlockInfo
{
	void*	_ptr;		// ָ���ڴ���ָ��
	string	_type;		// ����
	int		_num;		// ���ݸ���
	size_t	_size;		// ���ݿ��С

	string	_filename;	// �ļ���
	int		_fileline;	// �к�

	MemoryBlockInfo(void* ptr = 0, const char* type = "", int num = 0, size_t size = 0,
		const char* filename = "", int fileline = 0);

	// �������ݵ���������
	void Save(SaveAdapter& SA);
};

struct CountInfo
{
	int _addCount;		// ��Ӽ���
	int _delCount;		// ɾ������

	long long _totalSize;	// �����ܴ�С

	long long _usedSize;	// ����ʹ�ô�С
	long long _maxUsedSize;	// ���ʹ�ô�С(����ֵ)

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
	// ����������Ϣ��¼
	void AddRecord(const MemoryBlockInfo& memBlockInfo);
	void DelRecord(const MemoryBlockInfo& memBlockInfo);

	// ����������Ϣ��Ӧ��������
	void Save(SaveAdapter& SA);
	void OutPutLeakBlockInfo(SaveAdapter& SA);
	void OutPutHostObjectInfo(SaveAdapter& SA);
	void OutPutAllBlockInfo(SaveAdapter& SA);

	// ����������Ϣ
	static void Save();
	// ���������б������������
	void Clear();

private:
	MemoryAnalyse();
	~MemoryAnalyse();

	friend class Singleton<MemoryAnalyse>;

private:
	MemoryLeakMap	_leakBlockMap;			// �ڴ�й¶���¼
	MemoryHostMap	_hostObjectMap;			// �ȵ����(�Զ�������Ϊkey����ͳ��)
	MemoryBlcokInfos _memBlcokInfos;		// �ڴ������¼

	CountInfo		_allocCountInfo;		// ���������¼
};