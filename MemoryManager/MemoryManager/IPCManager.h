#pragma once

#include"CommonHeader.h"
#include"MMUtil.h"
#include"ConfigManager.h"
#include"MemoryAnalyse.h"


const int IPC_CONNECT_TIME_OUT = 1000;		// 连接管道超时时间
const int IPC_BUF_LEN = 4096;				// 数据缓冲区大小

#ifdef _WIN32
// 命名管道接收者
class NamePipeReceiver
{
public:
	NamePipeReceiver(const char* pipeName);
	~NamePipeReceiver();

	//监听
	bool Listen();

	// 接收消息
	bool ReceiverMsg(char* buf, size_t len, size_t& msgLen);

	// 关闭接收者管道
	void Close();

protected:
	HANDLE _hPipe;		// 管道句柄
	string _pipeName;	// 管道名
};

// 管道发送者
class NamePipeSender
{
public:
	NamePipeSender(const char* pipeName);
	~NamePipeSender();

	// 连接接受者
	bool Connect();

	// 发送消息
	bool SendMsg(const char* buf, size_t len, size_t& msgLen);

	// 关闭管道对象
	void Close();

protected:
	HANDLE _hPipe;
	string _pipeName;
};

#else
// 实现Linux版本命名管道接收者和发送者？
class NamePipeReceiver
{};

class NamePipeSender
{};
#endif

// IPC客户端
class IPCClient
{
public:
	IPCClient(const char* clientName, const char* serverName);
	~IPCClient();

	// 发送消息给服务端
	void SendMsg(char* buf, size_t len, size_t& msgLen);

	// 监听服务器的回复消息
	void ListenReply();

	// 获取服务端回复消息
	void GetReplyMsg(char* buf, size_t len, size_t& msgLen);

private:
	NamePipeSender _sender;			// 发送者
	NamePipeReceiver _reply;		// 回复者(接收服务端消息)
};

// IPC服务端
class IPCServer
{
public:
	IPCServer(const char* clientName, const char* serverName);
	~IPCServer();

	void Listen();

	// 接收客户端消息
	void ReceiverMsg(char* buf, size_t len, size_t& msgLen);

	// 回复客户端消息
	void SendReplyMsg(const char* buf, size_t len, size_t& msgLen);

private:
	NamePipeReceiver _receiver;		// 接收者
	NamePipeSender	 _reply;		// 回复者(回复消息给客户端)
};

class IPCOnlineServer : public Singleton<IPCOnlineServer>
{
	typedef void(*CmdFunc) (string& reply);
	typedef map<string, CmdFunc> CmdFuncMap;

public:
	// 启动IPC消息处理服务线程
	void Start();

	// IPC服务线程处理消息的函数
	void OnMessage();

protected:
	// 消息命令执行函数
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
	IPCServer _IPCServer;			// IPC服务
	thread _onMsgThread;			// 处理消息线程
	CmdFuncMap _cmdFuncsMap;		// 消息命令到执行函数的映射表
};