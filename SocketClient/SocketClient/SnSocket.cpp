#include "SnSocket.h"

SnSocket::SnSocket()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(1, 1), &wsaData);
}

SnSocket::~SnSocket()
{
	DebugPrint("~SnSocket", true);
	WSACleanup();
}

SOCKET SnSocket::SocketCreateTcp()
{
	local = socket(AF_INET, SOCK_STREAM, NULL);
	DebugPrint("SocketCreateTcp", local);
	return local;
}

void SnSocket::SocketBind(SnAddress addr)
{
	ZeroMemory(&addr.sai, sizeof(addr.sai));
	addr.sai.sin_family = AF_INET;
	addr.sai.sin_port = htons(addr.port);
	addr.sai.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	int result = bind(local, (sockaddr*)&addr.sai, sizeof(addr.sai));
	DebugPrint("SocketBind", !result);
}

void SnSocket::SocketListen(int backlog)
{
	int result = listen(local, backlog);
	DebugPrint("SocketListen", !result);
}

SOCKET SnSocket::SocketAccept(SnAddress* addr)
{
	int addrlen = sizeof(addr->sai);
	SOCKET link = accept(local, (sockaddr*)&addr->sai, &addrlen);
	addr->port = addr->sai.sin_port;
	sprintf_s(addr->ipaddr, "%s", inet_ntoa(addr->sai.sin_addr));
	DebugPrint("SocketAccept", link);
	return link;
}
void SnSocket::SocketConnect(SnAddress addr)
{
	ZeroMemory(&addr.sai, sizeof(addr.sai));
	addr.sai.sin_family = AF_INET;
	addr.sai.sin_port = htons(addr.port);
	addr.sai.sin_addr.S_un.S_addr = inet_addr(addr.ipaddr);
	int result = connect(local, (sockaddr*)&addr.sai, sizeof(addr.sai));
	DebugPrint("SocketConnect", !result);
}
int SnSocket::SocketSendString(SOCKET sock, char* str, int len)
{
	if (sock == NULL)
		sock = local;

	int slen = send(sock, str, len, 0);
	int result = SocketRecvInt(sock);

	char log[255];
	sprintf_s(log, "发送(%d/%d)：%s\t客户端接收(%d)", slen, len, str, result);
	DebugPrint(log, true);

	if (result == len)
		return result;
	else
		return 0;
}
int SnSocket::SocketRecvString(SOCKET sock, char* str, int len)
{
	if (sock == NULL)
		sock = local;

	//清空buff 防止接收不到数据 造成buffer too small
	ZeroMemory(str, len);

	int result = recv(sock, str, len, 0);
	SocketSendInt(sock, result);

	char log[255];
	sprintf_s(log, "接收(%d/%d)：%s", result, len, str);
	DebugPrint(log, true);

	if (result == len)
		return result;
	else
		return 0;
}
void SnSocket::SocketSendInt(SOCKET sock, int value)
{
	if (sock == NULL)
		sock = local;

	int len = 4;
	int data = value;

	int slen = send(sock, (char*)&data, len, 0);
	int	result = recv(sock, (char*)&result, 4, 0);

	if (result == len)
		return;
	else
		return;
}
int SnSocket::SocketRecvInt(SOCKET sock)
{
	if (sock == NULL)
		sock = local;

	int len = 4;
	int tempInt = 0;

	int result = recv(sock, (char*)&tempInt, len, 0);
	send(sock, (char*)&result, 4, 0);

	if (result == len)
		return tempInt;
	else
		return 0;
}

void SnSocket::SocketSendFile(SOCKET sock, char* path)
{
	if (sock == NULL)
		sock = local;


	//读取文件
	ifstream ifs;
	ifs.open(path, ios::in | ios::binary);
	ifs.seekg(0, ios::end);
	int len = (int)ifs.tellg();
	ifs.seekg(0, ios::beg);

	//发送文件长度到客户端
	SocketSendInt(sock, len);

	char* buff = new char[SN_BLOCKSIZE]();

	int sendSum = 0;
	do
	{
		int currSize = len - sendSum > SN_BLOCKSIZE ? SN_BLOCKSIZE : len - sendSum;
		SocketSendInt(sock, currSize);

		ifs.read(buff, currSize);
		send(sock, buff, currSize, 0);
		int result = SocketRecvInt(sock);
		if (result == 1)
		{
			sendSum += currSize;
			/*char templog[255];
			sprintf_s(templog, "本次发送:%d\t总发送:%d/%d",  currSize, sendSum, len);
			DebugPrint(templog, true);*/
		}
		else
		{
			/*char templog[255];
			sprintf_s(templog, "本次发送:%d\t总发送:%d/%d\t重新发送", currSize, sendSum, len);
			DebugPrint(templog, true);*/
			ifs.seekg(sendSum);
		}
	} while (sendSum < len);

	delete[] buff;

	char log[255];
	sprintf_s(log, "发送文件(%d/%d):%s", sendSum, len, path);
	DebugPrint(log, true);
}

void SnSocket::SocketSendFile(SOCKET sock, char* buff, int len)
{
	if (sock == NULL)
		sock = local;

	//发送文件长度到客户端
	SocketSendInt(sock, len);

	int sendSum = 0;
	do
	{
		int currSize = len - sendSum > SN_BLOCKSIZE ? SN_BLOCKSIZE : len - sendSum;
		SocketSendInt(sock, currSize);
		send(sock, buff + sendSum, currSize, 0);
		int result = SocketRecvInt(sock);
		if (result == 1)
		{
			sendSum += currSize;
		}

		/*char templog[255];
		sprintf_s(templog, "本次发送:%d\t总发送:%d/%d\t重新发送", currSize, sendSum, len);
		DebugPrint(templog, true);*/

	} while (sendSum < len);

	char log[255];
	sprintf_s(log, "发送文件(%d/%d)", sendSum, len);
	DebugPrint(log, true);
}

void SnSocket::SocketRecvFile(SOCKET sock, char* path)
{
	if (sock == NULL)
		sock = local;

	int len = SocketRecvInt(sock);

	ofstream ofs;
	ofs.open(path, ios::out | ios::binary);

	char* buff = new char[SN_BLOCKSIZE]();
	int recvSum = 0;
	do
	{
		int sevSize = SocketRecvInt(sock);
		int lsize = recv(sock, buff, sevSize, 0);
		if (lsize == sevSize)
		{
			recvSum += sevSize;
			ofs.write(buff, sevSize);
			SocketSendInt(sock, 1);
		}
		else
		{
			SocketSendInt(sock, 2);
		}

		char templog[255];
		sprintf_s(templog, "本次接收:%d/%d\t总接收:%d/%d\t%.2f%%", lsize, sevSize, recvSum, len,((float)recvSum / (float)len) * 100 );
		DebugPrint(templog, true);

	} while (recvSum < len);
	ofs.close();

	delete[] buff;

	char log[255];
	sprintf_s(log, "接收文件(%d/%d):%s", recvSum, len, path);
	DebugPrint(log, true);
}

void SnSocket::SocketClose(SOCKET sock)
{
	int result = closesocket(sock);
	DebugPrint("SocketClose", !result);
}
void SnSocket::SocketClose()
{
	int result = closesocket(local);
	DebugPrint("SocketClose", !result);
}
void SnSocket::DebugPrint(const char* buff, bool res)
{
	if (debugMode)
	{
		if (res)
			printf("SUCCESS %s\n", buff);
		else
			printf("FAILD %s\n", buff);
	}
}
