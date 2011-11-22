#ifndef _KMIPOS_CHATSERVER_H_
#define _KMIPOS_CHATSERVER_H_

#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

#define RESULT_OK							0
#define RESULT_ERROR_WINSOCK				1
#define RESULT_ERROR_SOCKET					2
#define RESULT_SERVER_CAPACITY_EXCEEDED		3

#define RECV_BUF_SIZE	1024

typedef struct _chat_server_line
{
	IN_ADDR origin;
	char* line;
} CHATLINE;

typedef struct _chat_server
{
	SOCKET sock;
	CHATLINE** conversation;
	int totalConvSize;
	int currentConvIndex;
} CHATSERVER;

int init(int port, int maxLines, CHATSERVER* serv);
void close(CHATSERVER* serv);

int chat_listen(CHATSERVER* serv);

int add_line(const IN_ADDR* origin, const char* text, CHATSERVER* serv);
void remove_line(CHATSERVER* serv, int lineIndex);
char* sprint_line(const CHATSERVER* serv, int lineIndex, int* length);

#endif