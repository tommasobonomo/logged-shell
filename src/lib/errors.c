#include "errors.h"
#include "utilities.h"
#include <stdlib.h>
#include <stdio.h>

void error_fatal(int id, char *msg)
{
    switch (id)
    {
        case ERR_X:
            printf("ERROR: %s\n", msg);
            break;
        case ERR_BAD_ARGS:
            printf("ERROR: Bad arguments\n");
            printHelp();
            break;
        case ERR_BAD_ARG_X:
            printf("ERROR, bad argument: %s\n", msg);
            break;
        case ERR_UNKNOWN_ARG_X:
            printf("ERROR, unknown argument: %s\n", msg);
            break;
        default:
            printf("ERROR not specified: %s\n", msg);
            break;
    }

    exit(id);
}