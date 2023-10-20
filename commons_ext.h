#ifndef COMMONS_H
#define COMMONS_H

#include <stdbool.h>
#include <limits.h>

#define KEY_SEED "commons.h"
#define KEY_ID 42
#define MSG_MAX_L 500
#define NAME_MAX_L 8
#define EXIT_STRING "Q"
#define ADMIN_ID LONG_MAX
#define REG_COMMAND '0'
#define UNREG_COMMAND '1'
#define SIGNAL_COMMAND '2'

typedef struct
{
        long messageType;
        char messageText[MSG_MAX_L];
} IPC_message;

long hash(char* name, int length);
bool getString(char *string, int length, bool spaceAllowed);

#endif
