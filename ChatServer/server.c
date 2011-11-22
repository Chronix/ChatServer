#define _CRT_SECURE_NO_WARNINGS

#include <assert.h>
#include <stdio.h>

#include "server.h"

int init(int port, int maxLines, CHATSERVER* serv)
{
	WSADATA data;
	SOCKADDR_IN sockAddr;

	if (WSAStartup(WINSOCK_VERSION, &data)) return RESULT_ERROR_WINSOCK;

	serv->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (serv->sock == INVALID_SOCKET) return RESULT_ERROR_SOCKET;

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	if (bind(serv->sock, (SOCKADDR*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR) return RESULT_ERROR_SOCKET;

	serv->totalConvSize = maxLines;
	serv->currentConvIndex = 0;
	serv->conversation = (CHATLINE**)malloc(maxLines * sizeof(CHATLINE*));

	return RESULT_OK;
}

void close(CHATSERVER* serv)
{
	int i;

	closesocket(serv->sock);
	WSACleanup();

	for (i = 0; i < serv->currentConvIndex; ++i)
	{
		remove_line(serv, i);
	}
}

int chat_listen(CHATSERVER* serv)
{
	int addrLen = sizeof(SOCKADDR_IN);
	SOCKADDR_IN clientInfo;
	char buf[RECV_BUF_SIZE + 1];
	int i;

	if (listen(serv->sock, 10) == SOCKET_ERROR) return RESULT_ERROR_SOCKET;

	while (1)
	{
		SOCKET client = accept(serv->sock, (SOCKADDR*)&clientInfo, &addrLen);

		if (client == INVALID_SOCKET) return RESULT_ERROR_SOCKET;

		if (recv(client, buf, RECV_BUF_SIZE, 0) == SOCKET_ERROR) return RESULT_ERROR_SOCKET;

		add_line(&clientInfo.sin_addr, buf, serv);

		for (i = 0; i < serv->currentConvIndex; ++i)
		{
			int len;
			char* line = sprint_line(serv, i, &len);

			if (send(client, line, len, 0) == SOCKET_ERROR) return RESULT_ERROR_SOCKET;

			free(line);
		}

		closesocket(client);
	}

	return RESULT_OK;
}

int add_line(const IN_ADDR* origin, const char* text, CHATSERVER* serv)
{
	int index = serv->currentConvIndex++;

	if (index >= serv->totalConvSize)
	{
		serv->currentConvIndex--;
		return RESULT_SERVER_CAPACITY_EXCEEDED;
	}

	serv->conversation[index] = (CHATLINE*)malloc(sizeof(CHATLINE));
	serv->conversation[index]->origin.S_un = origin->S_un;
	serv->conversation[index]->line = (char*)calloc(strlen(text) + 1, sizeof(char));
	strcpy(serv->conversation[index]->line, text);

	return RESULT_OK;
}

void remove_line(CHATSERVER* serv, int lineIndex)
{
	assert(lineIndex < serv->currentConvIndex);
	free(serv->conversation[lineIndex]->line);
	free(serv->conversation[lineIndex]);
}

char* sprint_line(const CHATSERVER* serv, int lineIndex, int* length)
{
	CHATLINE* line = serv->conversation[lineIndex];
	char* textLine = (char*)calloc(1000, sizeof(char*));
	sprintf(textLine, "%s: %s", inet_ntoa(line->origin), line->line);
	*length = strlen(textLine);
	return textLine;
}