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
void registerErrorSignalHAndler(int signal);
void terminateSignalHandler(int signal);
void registerClient();
void sendMessage(char *name, char *message);
IPC_message constructMessage(char *receiver, char command, char *name, char *text);
long self_id;
int msgid;
bool signaled;
char self_name[NAME_MAX_L + 1];
IPC_message msg;

int main(int argc, char *argv[])
{
#ifdef DEBUG
	printf("Arguments:\n");
	for(int i = 0; i < argc; i++)
		printf("%s\n", argv[i]);
#endif
	if(argc != 2)
	{
		perror("Invalid argument number!\n");
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
	signal(SIGUSR2, registerErrorSignalHAndler);
	signal(SIGTERM, terminateSignalHandler);

	strcpy(self_name, argv[1]);
	self_id = hash(self_name, strlen(self_name));
	registerClient(self_name); //No error check as server will just kill client if unable to register

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
		
		if(signaled) //otherwise it was crashing after returning from signal handler
		{
			signaled = false;
			continue;
		}

		printf("Type message (max %d characters):\n", MSG_MAX_L);
		correct = getString(message, MSG_MAX_L, true);
		while(!correct)
		{
			printf("Something went wrong! Try again:\n");
			correct = getString(message, MSG_MAX_L, false);
		}

		if(!strcmp(message, EXIT_STRING)) break;

		if(signaled) //otherwise it was crashing after returning from signal handler
		{
			signaled = false;
			continue;
		}

		sendMessage(name, message);
	}
	printf("Goodbye!\n");
	return 0;
}

void signalHandler(int signal)
{
	if(msgrcv(msgid, &msg, sizeof(msg), self_id, 0) == -1)
	{
		perror("Error receiving message!\n");
		exit(1);
	}
    printf("----------Message received:----------\n\n%s:%s\n\n-------------End message-------------\n", msg.messageText.name, msg.messageText.text);
	signaled = true;
	return;
}

void registerClient()
{
	/*
	*	could cast to char pointer and 4 bytes would be treated as 4 chars and then recast back to int,
	*	but just calculating sprintf (why isn't there itoa on linux?!) and then atoi will allow easier human readability for debugging
	*	also that way I can just use same IPC_message struct
	*/
	char pid[6]; //standard pid max value is 32768 so I'm assuming 5 chars + '\0' 
	sprintf(pid, "%d", getpid());
	IPC_message msg = constructMessage(ADMIN_NAME, '0', self_name, pid); 
	if(msgsnd(msgid, &msg, sizeof(msg), 0) == -1)
	{
		perror("Error sending message!\n");
		exit(1);
	}
#ifdef DEBUG
	printf("Sending: \"%s\", to server!\n", msg.messageText.name);
#endif
}

void sendMessage(char *name, char *message)
{
	IPC_message admMsg = constructMessage(ADMIN_NAME, '2', name, "");
	IPC_message msg = constructMessage(name, '2', self_name, message);

	if(msgsnd(msgid, &admMsg, sizeof(admMsg), 0) == -1)
	{
		perror("Error sending message!\n");
		exit(1);
	}
	if(msgsnd(msgid, &msg, sizeof(msg), 0) == -1)
	{
		perror("Error sending message!\n");
		exit(1);
	}
#ifdef DEBUG
	printf("Sending: \"%c:%s:%s\", to server!\n", admMsg.messageText.command, admMsg.messageText.name, admMsg.messageText.text);
	printf("Sending: \"%s\", to: %s\n", msg.messageText.text, msg.messageText.name);
#endif
}

void registerErrorSignalHAndler(int signal)
{
	printf("Unable to register, exiting!\n");
	exit(1);
}

void terminateSignalHandler(int signal)
{
	printf("Server requested termination, exiting!\n");
	exit(1);
}

IPC_message constructMessage(char *receiver, char command, char *name, char *text)
{
	IPC_message msg;
	if(!strcmp(ADMIN_NAME, receiver))
	{
		msg.messageType = ADMIN_ID;
	}
	else
	{
		msg.messageType = hash(receiver, strlen(receiver));
	}
	msg.messageText.command = command;
	strcpy(msg.messageText.name, name);
	strcpy(msg.messageText.text, text);
	return msg;
}