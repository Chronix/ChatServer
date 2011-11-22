#include <stdio.h>

#include "server.h"

CHATSERVER server;

void die(void)
{
	close(&server);
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("Zadejte port (napr. ChatServer.exe 12345)\n");
		return 1;
	}

	if (init(atoi(argv[1]), 100, &server) != RESULT_OK)
	{
		printf("Chyba pri incializaci serveru.\n");
		return 1;
	}

	atexit(die);	
	
	chat_listen(&server);

	return 0;
}