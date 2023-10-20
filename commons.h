#ifndef COMMONS_H
#define COMMONS_H

#include <stdbool.h>

#define KEY_SEED "commons.h"
#define KEY_ID 42
#define MSG_MAX_L 500
#define NAME_MAX_L 8
#define EXIT_STRING "Q"

typedef struct
{
        long messageType;
        char messageText[MSG_MAX_L];
} IPC_message;

long hash(char* name, int length);
bool getString(char *string, int length, bool spaceAllowed);

#endif
