#include"IPCManager.h"

// �Զ���������ͷŵ�����ָ��
template<class T>
class AutoRelease
{
public:
	AutoRelease(T* ptr, bool isArray = true)
		:_ptr(ptr)
		, _isArray(isArray)
	{}

	~AutoRelease()
	{
		if (_isArray)
		{
			delete[] _ptr;
		}
		else
		{
			delete _ptr;
		}
	}

	T* Get()
	{
		return _ptr;
	}

private:
	AutoRelease(const AutoRelease&);
	AutoRelease& operator=(const AutoRelease&);
private:
	T* _ptr;
	bool _isArray;
};

#ifdef _WIN32

char* __WideCharToMultiByte(const wchar_t* wcPtr)
{

	DWORD len = WideCharToMultiByte(CP_ACP, NULL, wcPtr,
		-1, NULL, 0, NULL, FALSE);

	char* cPtr = new char[len];

	WideCharToMultiByte(CP_ACP, NULL, wcPtr, -1, cPtr, len, NULL, FALSE);
	return cPtr;
}

wchar_t* __MultiByteToWideChar(const char* cPtr)
{
	DWORD len = MultiByteToWideChar(CP_ACP, 0, cPtr, -1, NULL, 0);

	wchar_t* wPtr = new wchar_t[len + 2];
	DWORD nLen = MultiByteToWideChar(CP_ACP, 0, cPtr, -1, wPtr, len);
	wPtr[nLen] = 0;
	return wPtr;
}

// ��¼windows�µĴ�����־
void RecordErrorLog(const char* errMsg, int line)
{
	printf("%s. ErrorId:%d, Line:%d\n", errMsg, GetLastError(), line);
}

#define RECORD_ERROR_LOG(errMsg)	\
	RecordErrorLog(errMsg, __LINE__)

////////////////////////////////////////////////////////////////
// NamePipeReceiver

NamePipeReceiver::NamePipeReceiver(const char* pipeName)
:_hPipe(INVALID_HANDLE_VALUE)
, _pipeName(pipeName)
{}

NamePipeReceiver::~NamePipeReceiver()
{
	Close();
}

bool NamePipeReceiver::Listen()
{
	// ��������ǰ�ȹرչܵ�����
	Close();

	AutoRelease<wchar_t> wPipeName = __MultiByteToWideChar(_pipeName.c_str());

	_hPipe = ::CreateNamedPipe(wPipeName.Get(),
		// �ܵ��ȿ��Է���Ҳ���Խ������� | �ܵ�����ͬ�����ͺͽ�������
		PIPE_ACCESS_DUPLEX | FILE_FLAG_WRITE_THROUGH,
		// �����ֽ��� | �����ֽ��� | �ȴ�ģʽ
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		// �ܵ��ĵ��������������
		PIPE_UNLIMITED_INSTANCES,
		IPC_BUF_LEN,
		IPC_BUF_LEN,
		// �ڵȴ�����ʱ��ĵȴ�ʱ��
		IPC_CONNECT_TIME_OUT,
		(LPSECURITY_ATTRIBUTES)NULL);

	if (_hPipe == INVALID_HANDLE_VALUE) {
		return false;
	}

	return true;
}

bool NamePipeReceiver::ReceiverMsg(char* buf, size_t len, size_t& msgLen)
{
	assert(_hPipe != INVALID_HANDLE_VALUE);
	assert(buf && len > 0);

	if (::ConnectNamedPipe(_hPipe, NULL) == FALSE) {
		return false;
	}

	DWORD readLen = 0;
	if (::ReadFile(_hPipe, buf, len, &readLen, NULL) == FALSE)
	{
		return false;
	}

	buf[readLen] = '\0';
	msgLen = readLen;

	return true;
}

void NamePipeReceiver::Close()
{
	if (_hPipe == INVALID_HANDLE_VALUE)
	{
		return;
	}

	if (::DisconnectNamedPipe(_hPipe) == 0)
	{
		printf("DisconnectNamedPipe Error. NamePipe:%s.\n",
			_pipeName.c_str());
	}

	if (::CloseHandle(_hPipe) == FALSE)
	{
		printf("CloseHandle Error. NamePipe:%s.\n", _pipeName.c_str());
	}

	_hPipe = INVALID_HANDLE_VALUE;
}

////////////////////////////////////////////////////////////////////////////////
// NamePipeSender

NamePipeSender::NamePipeSender(const char* pipeName)
:_hPipe(INVALID_HANDLE_VALUE)
, _pipeName(pipeName)
{}

NamePipeSender::~NamePipeSender()
{
	Close();
}

bool NamePipeSender::Connect()
{
	// ����֮ǰ�ȹرվɹܵ�����
	Close();

	AutoRelease<wchar_t> wPipeName = __MultiByteToWideChar(_pipeName.c_str());
	const int connectTimeOut = 1000;

	if (!::WaitNamedPipe(wPipeName.Get(), connectTimeOut))
	{
		return false;
	}

	_hPipe = ::CreateFile(wPipeName.Get(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		(LPSECURITY_ATTRIBUTES)NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);

	if (_hPipe == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	return true;
}

bool NamePipeSender::SendMsg(const char* buf, size_t len, size_t& msgLen)
{
	assert(_hPipe != INVALID_HANDLE_VALUE);
	assert(buf && len > 0);

	DWORD writeLen = 0;
	AutoRelease<wchar_t> cPtr = __MultiByteToWideChar(buf);

	if (!WriteFile(_hPipe, buf, len, &writeLen, NULL))
	{
		return false;
	}

	msgLen = writeLen;

	return true;
}

void NamePipeSender::Close()
{
	if (_hPipe == INVALID_HANDLE_VALUE)
	{
		return;
	}

	if (::CloseHandle(_hPipe) == FALSE)
	{
		printf("CloseHandle Error. NamePipe:%s.\n", _pipeName.c_str());
	}

	_hPipe = INVALID_HANDLE_VALUE;
}
#endif

////////////////////////////////////////////////////////////////////////////
// IPCClient

IPCClient::IPCClient(const char* clientName, const char* serverName)
:_sender(serverName)
, _reply(clientName)
{}

IPCClient::~IPCClient()
{}

// ������Ϣ�������
void IPCClient::SendMsg(char* buf, size_t len, size_t& msgLen)
{
	if (!_sender.Connect())
	{
		RECORD_ERROR_LOG("Client Connect Server Error");
		return;
	}

	if (!_sender.SendMsg(buf, len, msgLen))
	{
		RECORD_ERROR_LOG("Client SendMsg Error");
	}
}

// �����������Ļظ���Ϣ
void IPCClient::ListenReply()
{
	if (!_reply.Listen())
	{
		RECORD_ERROR_LOG("Client Listen Server Reply Error");
	}
}

// ��ȡ����˻ظ���Ϣ
void IPCClient::GetReplyMsg(char* buf, size_t len, size_t& msgLen)
{
	if (!_reply.ReceiverMsg(buf, len, msgLen))
	{
		RECORD_ERROR_LOG("Client Get Server Reply Msg Error");
	}
}

///////////////////////////////////////////////////////////////////////
// IPCServer

IPCServer::IPCServer(const char* clientName, const char* serverName)
:_reply(clientName)
, _receiver(serverName)
{}

IPCServer::~IPCServer()
{}

void IPCServer::Listen()
{
	if (!_receiver.Listen())
	{
		RECORD_ERROR_LOG("Server Listen Client Msg Error");
	}
}

// ���տͻ�����Ϣ
void IPCServer::ReceiverMsg(char* buf, size_t len, size_t& msgLen)
{
	if (!_receiver.ReceiverMsg(buf, len, msgLen))
	{
		RECORD_ERROR_LOG("Server Receiver Client Msg Error");
	}
}

// �ظ��ͻ�����Ϣ
void IPCServer::SendReplyMsg(const char* buf, size_t len, size_t& msgLen)
{
	if (!_reply.Connect())
	{
		RECORD_ERROR_LOG("Server Connect Client Reply Error");
		return;
	}

	if (!_reply.SendMsg(buf, len, msgLen))
	{
		RECORD_ERROR_LOG("Server Send Msg To Client Reply Error");
	}
}

///////////////////////////////////////////////////////////////////////////
// IPCOnlineServer

int GetProcessId()
{
	int processId = 0;

#ifdef _WIN32
	processId = GetProcessId(GetCurrentProcess());
#else
	processId = getpid();
#endif

	return processId;
}

string GetServerPipeName()
{
	string name = "\\\\.\\Pipe\\ServerPipeName";
	char idStr[10];
	_itoa(GetProcessId(), idStr, 10);
	name += idStr;

	return name;
}

string GetClientPipeName()
{
	string name = "\\\\.\\Pipe\\ClientPipeName";
	char idStr[10];
	_itoa(GetProcessId(), idStr, 10);
	name += idStr;

	return name;
}

IPCOnlineServer::IPCOnlineServer()
:_IPCServer(GetClientPipeName().c_str(), GetServerPipeName().c_str())
, _onMsgThread(&IPCOnlineServer::OnMessage, this)
{
	printf("%s IPC Online Server Start\n", GetServerPipeName().c_str());
	_cmdFuncsMap["state"] = GetState;
	_cmdFuncsMap["save"] = Save;
	_cmdFuncsMap["print"] = Print;
	_cmdFuncsMap["disable"] = Disable;
	_cmdFuncsMap["enable"] = Enable;
	_cmdFuncsMap["clear"] = Clear;
}

IPCOnlineServer::~IPCOnlineServer()
{}

void IPCOnlineServer::Start()
{
	//printf("IPC Online Server Thread Start\n");
}

void IPCOnlineServer::OnMessage()
{
	char msg[IPC_BUF_LEN];
	memset(msg, 0, IPC_BUF_LEN);
	size_t msgLen = 0;

	while (1)
	{
		_IPCServer.Listen();
		printf("Server Is Listening!\n");

		_IPCServer.ReceiverMsg(msg, IPC_BUF_LEN, msgLen);
		printf("Receiver Cmd Msg: %s\n", msg);

		string reply;
		string cmd = msg;
		CmdFuncMap::iterator it = _cmdFuncsMap.find(cmd);
		if (it != _cmdFuncsMap.end())
		{
			CmdFunc func = it->second;
			func(reply);
		}
		else
		{
			reply = "Invalid Command";
		}

		_IPCServer.SendReplyMsg(reply.c_str(), reply.size(), msgLen);
	}
}

void IPCOnlineServer::GetState(string& reply)
{
	reply += "State:";
	int flag = ConfigManager::GetInstance()->GetOptions();

	if (flag == CO_NONE)
	{
		reply += "None\n";
		return;
	}

	if (flag & CO_ANALYSE_MEMORY_LEAK)
	{
		reply += "Analyse Memory Leak\n";
	}

	if (flag & CO_ANALYSE_MEMORY_HOST)
	{
		reply += "Analyse Memory Host\n";
	}

	if (flag & CO_ANALYSE_MEMORY_ALLOC_INFO)
	{
		reply += "Analyse Memory Alloc Info\n";
	}

	if (flag & CO_SAVE_TO_CONSOLE)
	{
		reply += "Save To Console\n";
	}

	if (flag & CO_SAVE_TO_FILE)
	{
		reply += "Save To File\n";
	}
}

void IPCOnlineServer::Enable(string& reply)
{
	ConfigManager::GetInstance()->SetOptions(CO_ANALYSE_MEMORY_HOST
		| CO_ANALYSE_MEMORY_LEAK
		| CO_ANALYSE_MEMORY_ALLOC_INFO
		| CO_SAVE_TO_FILE);

	reply += "Enable Success";
}

void IPCOnlineServer::Disable(string& reply)
{
	ConfigManager::GetInstance()->SetOptions(CO_NONE);

	reply += "Disable Success";
}

void IPCOnlineServer::Save(string& reply)
{
	const string& path = ConfigManager::GetInstance()->GetOutputPath();
	MemoryAnalyse::GetInstance()->Save(FileSaveAdapter(path.c_str()));

	reply += "Save Success";
}

void IPCOnlineServer::Print(string& reply)
{
	// ����Ӧ�ðѽ�������Ϣ���ظ��ܵ��ͻ��ˣ�
	MemoryAnalyse::GetInstance()->Save(ConsoleSaveAdapter());

	reply += "Print Success";
}

void IPCOnlineServer::Clear(string& reply)
{
	MemoryAnalyse::GetInstance()->Clear();

	reply += "Clear Success";
}