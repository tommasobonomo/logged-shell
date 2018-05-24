#include "errors.h"
#include "utilities.h"
#include "../lib/syscalls.h"
#include <stdio.h>
#include <stdlib.h>

void error_fatal(int id, char const *msg)
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
        case ERR_IO_FILE:
            fprintf(stderr, "ERROR, can't open file: %s\n", msg);
            perror(" --> ");
            break;
        case ERR_SYSCALL:
            fprintf(stderr, "ERROR: %s\n", msg);
            perror(" --> ");
            break;
        case ERR_CHILD:
            fprintf(stderr, "ERROR on child doing: %s\n", msg);
            break;
        default:
            fprintf(stderr, "ERROR not specified: %s\n", msg);
            break;
    }
    exitAndNotifyDaemon(id);
}

void error_warning(int id, char const *msg)
{
    switch (id)
    {
        case ERR_SYSCALL:
            fprintf(stderr, "WARNING: %s\n", msg);
            perror(" --> ");
            break;
        default:
            fprintf(stderr, "WARNING not specified: %s\n", msg);
            break;
    }
}