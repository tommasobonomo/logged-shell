#include "errors.h"
#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>

void error_fatal(int id, char *msg)
{
    switch (id)
    {
    case ERR_X:
        fprintf(stderr, "ERROR: %s\n", msg);
        break;
    case ERR_BAD_ARGS:
        fprintf(stderr, "ERROR: Bad arguments\n");
        printHelp();
        break;
    case ERR_BAD_ARG_X:
        fprintf(stderr, "ERROR, bad argument: %s\n", msg);
        break;
    case ERR_UNKNOWN_ARG_X:
        fprintf(stderr, "ERROR, unknown argument: %s\n", msg);
        break;
    default:
        fprintf(stderr, "ERROR not specified: %s\n", msg);
        break;
    }

    exit(id);
}