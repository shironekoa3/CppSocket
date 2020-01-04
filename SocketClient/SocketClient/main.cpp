#include <stdio.h>
#include <Windows.h>
#include <winsock.h>
#include <fstream>
#include "SnSocket.h"
#pragma comment(lib,"ws2_32.lib")
using namespace std;

int main()
{
	SnSocket sn;

	sn.SocketCreateTcp();

	SnAddress sa;
	sa.port = 3333;
	sprintf_s(sa.ipaddr, "203.195.166.101");
	//sprintf_s(sa.ipaddr, "127.0.0.1");

	sn.SocketConnect(sa);


	char buff[255];
	sn.SocketRecvString(NULL, buff, sizeof(buff));
	printf("%s\n", buff);
	sprintf_s(buff, "�ظ�һ�������ı�!");
	sn.SocketSendString(NULL, buff, sizeof(buff));

	int data = sn.SocketRecvInt(NULL);
	printf("���������� %d\n", data);
	sn.SocketSendInt(NULL, 776);

	int c = GetTickCount();

	sn.SocketRecvFile(NULL, (char*)"F:\\12.zip");

	c = GetTickCount() - c;


	sn.SocketClose();

	printf("���!\t%.2fs\n", ((float)c / 1000));
	getchar();
	return 0;
}


