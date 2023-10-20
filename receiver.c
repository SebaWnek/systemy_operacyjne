#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include "commons.h"

int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		fprintf(stderr, "Invalid argument number!\n");
		return 1;
	}

	for(int i = 0; i < argc; i++)
		printf("%s\n", argv[i]);

	char message[MSG_MAX_L];
	char *name = argv[1];
	IPC_message msg;
	key_t key = ftok(KEY_SEED, KEY_ID);
	int msgid = msgget(key, 0666);
	int number = atoi(argv[2]);

	printf("Message Queue ID: %d\n", msgid);

	msg.messageType = hash(name, strlen(name));

        printf("Receiving %d messages:\n", number);

	for(int i = 0; i < number; i++)
	{
		msgrcv(msgid, &msg, sizeof(msg), msg.messageType, 0);
		printf("Message received: %s\n", msg.messageText);
	}
	printf("Goodbye!\n");
	return 0;
}
