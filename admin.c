#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "commons.h"

int main()
{
	key_t key;
	int msgid;
	char input;

	printf("Message queue starting...\n");

	key = ftok(KEY_SEED, KEY_ID);
	msgid = msgget(key, 0666 | IPC_CREAT);

	printf("Message Queue ID: %d\n", msgid);
	printf("Message queue created!\nType Q key to stop:\n");

	do
	{
		input = getchar();
	} while (input != 'Q');
	

	msgctl(msgid, IPC_RMID, NULL);
	printf("Goodbye!\n");
	return 0;
}
