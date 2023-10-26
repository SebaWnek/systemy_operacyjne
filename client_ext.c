#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include "commons_ext.h"

void signalHandler(int signal);
void errorSignalHAndler(int signal);
int registerClient(char *name);
int sendMessage(char *name, char *message);
long self_id;
int msgid;
char self_name[NAME_MAX_L + 1];

int main(int argc, char *argv[])
{
	printf("Arguments:\n");
	for(int i = 0; i < argc; i++)
		printf("%s\n", argv[i]);

	if(argc != 2)
	{
		fprintf(stderr, "Invalid argument number!\n");
		return 1;
	}

	char message[MSG_MAX_L];
	char name[NAME_MAX_L + 1];
	bool correct;
	key_t key = ftok(KEY_SEED, KEY_ID);
	msgid = msgget(key, 0666);
	
	if(msgid == -1)
	{
		printf("Unable to find Queue!\n");
		return 1;
	}
	else printf("Message Queue ID: %d\n", msgid);

	signal(SIGUSR1, signalHandler);
	signal(SIGUSR2, errorSignalHAndler);

	strcpy(self_name, argv[1]);
	self_id = hash(self_name, strlen(self_name));
	registerClient(self_name);

    printf("Type %s to quit at any time!\n", EXIT_STRING);

	while(true)
	{
		printf("Type receiver name (max %d characters, no white space chars):\n", NAME_MAX_L);
		correct = getString(name, NAME_MAX_L, false);
		while(!correct)
		{
			printf("Name can't contain whitespace characters! Try again:\n");
			correct = getString(name, NAME_MAX_L, false);
		}

		if(!strcmp(name, EXIT_STRING)) break;

		printf("Type message (max %d characters):\n", MSG_MAX_L);
		correct = getString(message, MSG_MAX_L, false);
		while(!correct)
		{
			printf("Something went wrong! Try again:\n");
			correct = getString(message, MSG_MAX_L, false);
		}

		if(!strcmp(message, EXIT_STRING)) break;

		sendMessage(name, message);
	}
	printf("Goodbye!\n");
	return 0;
}

void signalHandler(int signal)
{
	IPC_message msg;
	msgrcv(msgid, &msg, sizeof(msg), self_id, 0);
    printf("Message received: \"%s\"\n", msg.messageText);
}

int registerClient(char *name)
{
	IPC_message msg;
	msg.messageType = ADMIN_ID;
	msg.messageText->command = REG_COMMAND;
	strcpy(msg.messageText->name,)
	sprintf(msg.messageText, "%c:%s,%d", REG_COMMAND, self_name, getpid());
	//strcpy(msg.messageText, strcat('0', ':', self_name, ',', itoa(getpid()))	//msg.messageText = strcat('0', ':', self_name, ',', itoa(getpid()));
	msgsnd(msgid, &msg, sizeof(msg), 0);
	printf("Sending: \"%s\", to server!\n", msg.messageText);
}

int sendMessage(char *name, char *message)
{
	IPC_message admMsg;
	IPC_message msg;

	admMsg.messageType	= ADMIN_ID;
	sprintf(admMsg.messageText, "%c:%s", SIGNAL_COMMAND, name);

	msg.messageType = (hash(name, strlen(name)));
	strcpy(msg.messageText, message);

	msgsnd(msgid, &admMsg, sizeof(admMsg), 0);
	msgsnd(msgid, &msg, sizeof(msg), 0);
	
	printf("Sending: \"%s\", to server!\n", admMsg.messageText);
	printf("Sending: \"%s\", to: %s, %ld\n", msg.messageText, name, msg.messageType);
}

void errorSignalHAndler(int signal)
{
	printf("Name already registered, exiting!\n");
	exit(1);
}