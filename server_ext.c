#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <signal.h>
#include "commons_ext.h"

/*
*	I'm using simple one byte commands for admin application
*	Command 0 - register client, 
*	Command 1 - unregister client, 
*	Command 2 - signal other client
*/

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
	int processPid;
} client_data;

client_data clients[MAX_CLIENTS];
int clients_count = 0;
int msgid;

int main()
{
	signal(SIGINT, exitHandler);
	
	key_t key;
	IPC_message current_msg;
	msg_data *data;

	printf("Message queue starting...\n");

	key = ftok(KEY_SEED, KEY_ID);
	msgid = msgget(key, 0666 | IPC_CREAT);

	printf("Message Queue ID: %d\n", msgid);
	printf("Message queue created!\n");

	while(true)
	{
        msgrcv(msgid, &current_msg, sizeof(current_msg), ADMIN_ID , 0);
        printf("Message received: %s\n", current_msg.messageText);
		data = (msg_data*)current_msg.messageText;
		
		switch(data->command)
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
				fprintf(stderr, "Incorrect command!\n");
		}
	}

	msgctl(msgid, IPC_RMID, NULL);
	printf("Goodbye!\n");
	return 0;
}

int registerClient(IPC_message *msg)
{
	int existing;
	if (clients_count >= MAX_CLIENTS)
	{
		fprintf(stderr, "Max clients number reached\n");
		return 1;
	}
	msg_data *data = (void*)msg->messageText;
	if(getPid(data->name) == -1) //check if name not already on a list
	{	
		strcpy(clients[clients_count].name, data->name);
		clients[clients_count].processPid = atoi(data->pid);
		printf("Registered client name: %s, pid: %d\n", clients[clients_count].name, clients[clients_count].processPid);
		clients_count++;
	}
	else
	{
		printf("Name %s already registered!\n", data->name);
		kill(atoi(data->pid),SIGUSR2);
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
	msg_data *data = (void*)msg->messageText;
	int pid = getPid(data->name);
	if(pid = -1)
	{
		printf("No client with \"%s\" name registered!\n", data->name);
		return 1;
	}
	kill(pid, SIGUSR1);
	printf("Signaled client name: %s, pid: %d\n", data->name, pid);
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
	msgctl(msgid, IPC_RMID, NULL);
	printf("\nBye!\n");
	exit(1);
}