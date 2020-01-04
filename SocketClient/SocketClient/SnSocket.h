#pragma once
#include <Windows.h>
#include <winsock.h>
#include <fstream>
#include <shlobj.h>
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "shell32.lib")
using namespace std;

//文件分块大小
#define SN_BLOCKSIZE 204800

struct SnAddress
{
	int port;
	char ipaddr[16];
	sockaddr_in sai;
};

class SnSocket
{
public:
	SOCKET  local = NULL;
	int lastError = 0;
	int debugMode = 0;

	SOCKET SocketCreateTcp();
	void SocketBind(SnAddress addr);
	void SocketListen(int backlog);
	SOCKET SocketAccept(SnAddress* addr);
	void SocketConnect(SnAddress addr);
	int SocketSendString(SOCKET sock, char* str, int len);
	int SocketRecvString(SOCKET sock, char* str, int len);
	void SocketSendInt(SOCKET sock, int value);
	int SocketRecvInt(SOCKET sock);
	void SocketSendFile(SOCKET sock, char* path);
	void SocketSendFile(SOCKET sock, char* buff, int len);
	void SocketRecvFile(SOCKET sock, char* path);
	void SocketClose(SOCKET sock);
	void SocketClose();
	void DebugPrint(const char* buff, bool res);

private:

public:
	SnSocket();
	~SnSocket();

};

