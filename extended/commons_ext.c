#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include "commons_ext.h"

#define HASH_BASE 4

/*
*       Assuming all ASCI characters availible their values are between 33 and 126 inclusive.
*       Then for every n-th char 4 (BASE) times it's value will be always more than n-1'st character.
*       I.e. adding '!' at 1st position will add 132, where max value from first position will be 127.
*       In that case there will never be possibility to generate the same hash value for different char combinations.
*/
long hash(char* name, int length)
{
        long result = 0;

        if(length > NAME_MAX_L)
        {
                length = NAME_MAX_L;
        }

        for(int i = 0; i < length; i++)
        {
                result += name[i] * pow(HASH_BASE, i);
        }
        return result;
}

//Returns false if incorrect string, so main can repeat question
bool getString(char *string, int length, bool spaceAllowed)
{
        char c;
        int i = 0;

        while ((c = getchar()) != '\n' && c != EOF)
        {
                if(!spaceAllowed && isspace(c))
                {
                        perror("String can't contain white space characters!\n");
                        return false;
                }
                if (i < length)
                        string[i++] = c;
        }
        string[i] = '\0';
	return true;
}
