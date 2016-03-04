#include"MemoryManager.h"
#include"ConfigManager.h"

// 申请&释放对象
// @parm type 申请类型
//#define ALLOC_TYPE(type)					\
//	__ALLOC_TYPE<type>(__FILE__, __LINE__);

#define ALLOC_TYPE(type)					\
	new(MemoryManager::GetInstance()->Alloc \
	(sizeof(type), typeid(type).name(), 1, __FILE__, __LINE__)) type

// @parm ptr 待释放对象的指针
#define DEALLOC_TYPE(ptr)					\
	__DEALLOC_TYPE(ptr)

// 申请&释放对象数组
// @parm type 申请类型
// @parm num 申请对象个数
#define ALLOC_TYPE_ARRAY(type, num)			\
	__ALLOC_TYPE_ARRAY<type>(num, __FILE__, __LINE__)

// @parm ptr 待释放对象的指针
#define DEALLOC_TYPE_ARRAY(ptr)				\
	__DEALLOC_TYPE_ARRAY(ptr)

// 设置配置选项
// @parm flag 配置选项
#define SET_MMA_OPTIONS(flag)				\
	ConfigManager::GetInstance()->SetOptions(flag);

// 设置剖析结果保存路径
// @parm path 保存路径
#define SET_MMA_OUTPUT_PATH(path)			\
	ConfigManager::GetInstance()->SetOutputPath(path);

// 保存剖析数据
#define MMA_SAVE()							\
	MemoryAnalyse::GetInstance()->Save();

// 清除剖析数据
#define MMA_CLEAR()							\
	MemoryAnalyse::GetInstance()->Clear();