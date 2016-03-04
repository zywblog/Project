#pragma once

#include"MMUtil.h"

// ps:���ͨ�������ļ�����������Ϣ�Ĺ��ܣ�


// ����ѡ��
enum ConfigOptions
{
	CO_NONE = 0,					// ��������
	CO_ANALYSE_MEMORY_LEAK = 1,		// �����ڴ�й¶
	CO_ANALYSE_MEMORY_HOST = 2,		// �����ڴ��ȵ�
	CO_ANALYSE_MEMORY_ALLOC_INFO = 4,// �����ڴ��������
	CO_SAVE_TO_CONSOLE = 8,			// ���浽����̨
	CO_SAVE_TO_FILE = 16,			// ���浽�ļ�
};

class ConfigManager : public Singleton<ConfigManager>
{
public:
	void SetOptions(int flag);
	int GetOptions();
	void SetOutputPath(const string& path);
	const string& GetOutputPath();

protected:
	ConfigManager();
	virtual ~ConfigManager();

	friend class Singleton<ConfigManager>;

private:
	int _flag;			// ѡ����	
	string _outputPath; // ���·��
};
