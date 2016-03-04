#pragma once

#include"MMUtil.h"

// ps:添加通过配置文件配置剖析信息的功能？


// 配置选项
enum ConfigOptions
{
	CO_NONE = 0,					// 不做剖析
	CO_ANALYSE_MEMORY_LEAK = 1,		// 剖析内存泄露
	CO_ANALYSE_MEMORY_HOST = 2,		// 剖析内存热点
	CO_ANALYSE_MEMORY_ALLOC_INFO = 4,// 剖析内存块分配情况
	CO_SAVE_TO_CONSOLE = 8,			// 保存到控制台
	CO_SAVE_TO_FILE = 16,			// 保存到文件
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
	int _flag;			// 选项标记	
	string _outputPath; // 输出路径
};
