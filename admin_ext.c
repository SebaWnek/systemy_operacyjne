#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <signal.h>
#include "commons_ext.h"

/*
*	I'm using simple one byte commands for admin application, message has format of command:data[,mored data]
*	Command 0 - register client, data: name,pid - as char arrays for easier debugging
*	Command 1 - unregister client, data: name,pid
*	Command 2 - signal other client, data: other client's name - also as char array
*	Whole msg can be casted to msg_data struct for easier human readability
*/

#define MAX_CLIENTS 10
#define PID_MAX_CHAR 5 //I'm assuming max PID of 32768 as default one

int registerClient(IPC_message *msg);
int unregisterClient(IPC_message *msg);
int signalClient(IPC_message *msg);

typedef struct
{
	char name[NAME_MAX_L];
	int processPid;
} client_data;

typedef struct
{
	char command;
	char colon;
	char name[NAME_MAX_L];
	char coma;
	char pid[5];
} msg_data;

client_data clients[MAX_CLIENTS];
int clients_count = 0;

int main()
{
	key_t key;
	int msgid;
	char input[NAME_MAX_L];
	IPC_message current_msg;
	msg_data *data;

	printf("Message queue starting...\n");

	key = ftok(KEY_SEED, KEY_ID);
	msgid = msgget(key, 0666 | IPC_CREAT);

	printf("Message Queue ID: %d\n", msgid);
	printf("Message queue created!\nType Q key to stop:\n");

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
				fprintf(stderr, "Incorrect command!");
		}
	}

	msgctl(msgid, IPC_RMID, NULL);
	printf("Goodbye!\n");
	return 0;
}

int registerClient(IPC_message *msg)
{
	if (clients_count >= MAX_CLIENTS)
	{
		fprintf(stderr, "Max clients number reached");
		return 1;
	}
	msg_data *data = &(msg->messageText);
	strcpy(clients[clients_count].name, data->name);
	clients[clients_count].processPid = atoi(data->pid);
	clients_count++;
	return 0;
}

int unregisterClient(IPC_message *msg)
{
	//TBD as this will require moving structs in array when removing not last one, maybe could use linked list?
}

//Using SIGUSR1 to inform client about incoming message
int signalClient(IPC_message *msg)
{
	msg_data *data = &(msg->messageText);
	int pid = atoi(data->pid);
	kill(pid, SIGUSR1);
	return 0;
}
