#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <signal.h>
#include "commons_ext.h"



#define MAX_CLIENTS 10
#define PID_MAX_CHAR 6 //I'm assuming max PID of 32768 as default one + '\0'

int registerClient(IPC_message *msg);
int unregisterClient(IPC_message *msg);
int signalClient(IPC_message *msg);
int getPid(char *name);
void exitHandler(int signal);

typedef struct
{
	char name[NAME_MAX_L + 1];
	__pid_t processPid;
} client_data;

client_data clients[MAX_CLIENTS];
int clients_count = 0;
int msgid;

int main()
{
	signal(SIGINT, exitHandler);
	
	IPC_message current_msg;
	key_t key;
	msg_data *data;
#ifdef DEBUG
	printf("Message queue starting...\n");
#endif
	key = ftok(KEY_SEED, KEY_ID);
	msgid = msgget(key, 0666 | IPC_CREAT);
	if(msgid == -1)
	{
		perror("Unable to create queue!");
		exit(1);
	}
	printf("Message Queue ID: %d\n", msgid);
#ifdef DEBUG
	printf("Message queue created!\n");
#endif

	while(true)
	{
		if(msgrcv(msgid, &current_msg, sizeof(current_msg), ADMIN_ID , 0) == -1)
		{
			perror("Error receiving message!");
			exit(1);
		}
        printf("Message received: %c:%s:%s\n", current_msg.messageText.command, current_msg.messageText.name, current_msg.messageText.text);
		
		switch(current_msg.messageText.command)
		{
			case REG_COMMAND:
				registerClient(&current_msg);
				break;
			case UNREG_COMMAND:
				unregisterClient(&current_msg);
				break;
			case SIGNAL_COMMAND:
				signalClient(&current_msg);
				break;
			default:
				perror("Incorrect command!\n");
		}
	}

	msgctl(msgid, IPC_RMID, NULL);
	printf("Goodbye!\n");
	return 0;
}

int registerClient(IPC_message *msg)
{
	int existing;
	__pid_t pid = atoi(msg->messageText.text);
	if (clients_count >= MAX_CLIENTS)
	{
		perror("Max clients number reached\n");
		//Using SIGUSR2 to signal client it wasn't able to be registered
		kill(pid,SIGUSR2);
		return 1;
	}

	if(getPid(msg->messageText.name) == -1) //check if name not already on a list, -1 means not found
	{	
		strcpy(clients[clients_count].name, msg->messageText.name);
		clients[clients_count].processPid = pid;
		printf("Registered client name: %s, pid: %d\n", clients[clients_count].name, clients[clients_count].processPid);
		clients_count++;
	}

	else
	{
		printf("Name %s already registered!\n", msg->messageText.name);
		kill(pid,SIGUSR2);
		return 1;
	}

	return 0;
}

int unregisterClient(IPC_message *msg)
{
	//TBD as this will require moving structs in array when removing not last one, maybe could use linked list?
}

//Using SIGUSR1 to inform client about incoming message
int signalClient(IPC_message *msg)
{
	int pid = getPid(msg->messageText.name);
	if(pid == -1)
	{
		fprintf(stderr, "No client with \"%s\" name registered!\n", msg->messageText.name);
		return 1;
	}
	kill(pid, SIGUSR1);
	printf("Signaled client name: %s, pid: %d\n", msg->messageText.name, pid);
	return 0;
}

int getPid(char *name)
{
	int found = 1;
	int result = -1;
	for(int i = 0; i < clients_count; i++)
	{
		found = strcmp(name, clients[i].name);
		if(found == 0)
		{
			return clients[i].processPid;
		}
	}
	return -1; //not found
}

void exitHandler(int signal)
{
	//Using SIGTERM to terminate all clients
	for(int i = 0; i < clients_count; i++)
	{
		kill(clients[i].processPid, SIGTERM);
	}
	msgctl(msgid, IPC_RMID, NULL);
	printf("\nBye!\n");
	exit(1);
}