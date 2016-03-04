#include"MemoryManager.h"
#include"ConfigManager.h"

// ����&�ͷŶ���
// @parm type ��������
//#define ALLOC_TYPE(type)					\
//	__ALLOC_TYPE<type>(__FILE__, __LINE__);

#define ALLOC_TYPE(type)					\
	new(MemoryManager::GetInstance()->Alloc \
	(sizeof(type), typeid(type).name(), 1, __FILE__, __LINE__)) type

// @parm ptr ���ͷŶ����ָ��
#define DEALLOC_TYPE(ptr)					\
	__DEALLOC_TYPE(ptr)

// ����&�ͷŶ�������
// @parm type ��������
// @parm num ����������
#define ALLOC_TYPE_ARRAY(type, num)			\
	__ALLOC_TYPE_ARRAY<type>(num, __FILE__, __LINE__)

// @parm ptr ���ͷŶ����ָ��
#define DEALLOC_TYPE_ARRAY(ptr)				\
	__DEALLOC_TYPE_ARRAY(ptr)

// ��������ѡ��
// @parm flag ����ѡ��
#define SET_MMA_OPTIONS(flag)				\
	ConfigManager::GetInstance()->SetOptions(flag);

// ���������������·��
// @parm path ����·��
#define SET_MMA_OUTPUT_PATH(path)			\
	ConfigManager::GetInstance()->SetOutputPath(path);

// ������������
#define MMA_SAVE()							\
	MemoryAnalyse::GetInstance()->Save();

// �����������
#define MMA_CLEAR()							\
	MemoryAnalyse::GetInstance()->Clear();