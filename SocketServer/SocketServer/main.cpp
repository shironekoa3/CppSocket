
#include <stdio.h>
#include <winsock.h>
#include <fstream>
#include <shlobj.h>
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "shell32.lib")

#include "SnSocket.h"
using namespace std;

int main()
{
	SnSocket sn;

	sn.SocketCreateTcp();
	SnAddress sa;
	sa.port = 3333;
	sn.SocketBind(sa);
	sn.SocketListen(3);

	SnAddress clientAddr;
	SOCKET link = sn.SocketAccept(&clientAddr);
	printf("IP %s:%d ���ӳɹ�.\n", clientAddr.ipaddr, clientAddr.port);

	char buff[255] = "����һ�β����ı�!";
	sn.SocketSendString(link, buff, sizeof(buff));
	sn.SocketRecvString(link, buff, sizeof(buff));
	printf("%s\n", buff);


	sn.SocketSendInt(link, 998);
	int data = sn.SocketRecvInt(link);
	printf("���������� %d\n", data);


	char path[MAX_PATH] = "C:\\Users\\Administrator\\Desktop\\A.zip";
	//char path[MAX_PATH] = "E:\\SystemMoe\\NetSpeedMonitorCN.zip";
	sn.SocketSendFile(link, path);

	sn.SocketClose(link);
	sn.SocketClose();

	printf("���!\n");


	int tg = getchar();
	return 0;
}



