#pragma once

#include"CommonHeader.h"
#include"MMUtil.h"
#include"ConfigManager.h"
#include"MemoryAnalyse.h"


const int IPC_CONNECT_TIME_OUT = 1000;		// ���ӹܵ���ʱʱ��
const int IPC_BUF_LEN = 4096;				// ���ݻ�������С

#ifdef _WIN32
// �����ܵ�������
class NamePipeReceiver
{
public:
	NamePipeReceiver(const char* pipeName);
	~NamePipeReceiver();

	//����
	bool Listen();

	// ������Ϣ
	bool ReceiverMsg(char* buf, size_t len, size_t& msgLen);

	// �رս����߹ܵ�
	void Close();

protected:
	HANDLE _hPipe;		// �ܵ����
	string _pipeName;	// �ܵ���
};

// �ܵ�������
class NamePipeSender
{
public:
	NamePipeSender(const char* pipeName);
	~NamePipeSender();

	// ���ӽ�����
	bool Connect();

	// ������Ϣ
	bool SendMsg(const char* buf, size_t len, size_t& msgLen);

	// �رչܵ�����
	void Close();

protected:
	HANDLE _hPipe;
	string _pipeName;
};

#else
// ʵ��Linux�汾�����ܵ������ߺͷ����ߣ�
class NamePipeReceiver
{};

class NamePipeSender
{};
#endif

// IPC�ͻ���
class IPCClient
{
public:
	IPCClient(const char* clientName, const char* serverName);
	~IPCClient();

	// ������Ϣ�������
	void SendMsg(char* buf, size_t len, size_t& msgLen);

	// �����������Ļظ���Ϣ
	void ListenReply();

	// ��ȡ����˻ظ���Ϣ
	void GetReplyMsg(char* buf, size_t len, size_t& msgLen);

private:
	NamePipeSender _sender;			// ������
	NamePipeReceiver _reply;		// �ظ���(���շ������Ϣ)
};

// IPC�����
class IPCServer
{
public:
	IPCServer(const char* clientName, const char* serverName);
	~IPCServer();

	void Listen();

	// ���տͻ�����Ϣ
	void ReceiverMsg(char* buf, size_t len, size_t& msgLen);

	// �ظ��ͻ�����Ϣ
	void SendReplyMsg(const char* buf, size_t len, size_t& msgLen);

private:
	NamePipeReceiver _receiver;		// ������
	NamePipeSender	 _reply;		// �ظ���(�ظ���Ϣ���ͻ���)
};

class IPCOnlineServer : public Singleton<IPCOnlineServer>
{
	typedef void(*CmdFunc) (string& reply);
	typedef map<string, CmdFunc> CmdFuncMap;

public:
	// ����IPC��Ϣ��������߳�
	void Start();

	// IPC�����̴߳�����Ϣ�ĺ���
	void OnMessage();

protected:
	// ��Ϣ����ִ�к���
	static void GetState(string& reply);
	static void Disable(string& reply);
	static void Enable(string& reply);
	static void Save(string& reply);
	static void Print(string& reply);
	static void Clear(string& reply);

protected:
	IPCOnlineServer();
	~IPCOnlineServer();

	friend class Singleton<IPCOnlineServer>;
private:
	IPCServer _IPCServer;			// IPC����
	thread _onMsgThread;			// ������Ϣ�߳�
	CmdFuncMap _cmdFuncsMap;		// ��Ϣ���ִ�к�����ӳ���
};