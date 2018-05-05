#include <stdio.h>
#include <string.h>
#include "utilities.h"

void printHelp()
{
    //TODO build help
    printf("SUPER FANCY HELP\n");
}

bool strStartWith(const char *str, const char *prefix)
{
    return strncmp(str, prefix, strlen(prefix)) == 0 ? true : false;
}
