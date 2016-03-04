#include"ConfigManager.h"
#include"MemoryAnalyse.h"

ConfigManager::ConfigManager()
:_flag(CO_NONE)
, _outputPath("MMA_REPORT.txt")
{}

ConfigManager::~ConfigManager()
{}

void ConfigManager::SetOptions(int flag)
{
	_flag = flag;

	// �����õı���ѡ����ڳ����˳�ʱ�Զ������
	if (flag & CO_SAVE_TO_CONSOLE | flag & CO_SAVE_TO_FILE)
	{
		atexit(MemoryAnalyse::Save);
	}
}

int ConfigManager::GetOptions()
{
	return _flag;
}

void ConfigManager::SetOutputPath(const string& path)
{
	_outputPath = path;
}

const string& ConfigManager::GetOutputPath()
{
	return _outputPath;
}