#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "commons.h"

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		fprintf(stderr, "Invalid argument number!\n");
		return 1;
	}

	char message[MSG_MAX_L];
	char *name = argv[1];
	bool correct;
	IPC_message msg;
	key_t key = ftok(KEY_SEED, KEY_ID);
	int msgid = msgget(key, 0666);

	printf("Message Queue ID: %d\n", msgid);

	msg.messageType = hash(name, strlen(name));

	for(int i = 0; i < argc; i++)
		printf("%s\n", argv[i]);

        printf("Type %s to quit at any time!\n", EXIT_STRING);

	while(true)
	{
		printf("Type message (max %d characters):\n", MSG_MAX_L);
		correct = getString(message, MSG_MAX_L, true);
		while(!correct)
		{
			printf("Something went wrong! Try again:\n");
			correct = getString(message, MSG_MAX_L, true);
		}

		if(!strcmp(message, EXIT_STRING)) break;

		printf("Sending: %s, to: %s\n", message, name);

		strcpy(msg.messageText, message);
		msgsnd(msgid, &msg, sizeof(msg), 0);
	}
	printf("Goodbye!\n");
	return 0;
}
