#ifndef COMMONS_H
#define COMMONS_H

#include <stdbool.h>
#include <limits.h>

#define KEY_SEED "commons.h"
#define KEY_ID 42
#define MSG_MAX_L 500
#define NAME_MAX_L 8
#define EXIT_STRING "Q"
#define ADMIN_NAME "ADMIN"
#define ADMIN_ID LONG_MAX
#define REG_COMMAND '0'
#define UNREG_COMMAND '1'
#define SIGNAL_COMMAND '2'

/*
*	I'm using simple one byte commands for admin application
*	Command 0 - register client, 
*	Command 1 - unregister client, 
*	Command 2 - message to other client
*       "name" can be used either for sending own name to another client, or to send target name for server to signal
*/

typedef struct
{
	char command;
	char name[NAME_MAX_L + 1];
        char text[MSG_MAX_L];
} msg_data;

typedef struct
{
        long messageType;
        msg_data messageText;
} IPC_message;


long hash(char* name, int length);
bool getString(char *string, int length, bool spaceAllowed);

#endif
