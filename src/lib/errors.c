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
            fprintf(stderr, COLOR_RED"ERROR: %s\n"COLOR_NORMAL, msg);
            break;
        case ERR_BAD_ARGS:
            fprintf(stderr, COLOR_RED"ERROR: Bad arguments\n"COLOR_NORMAL);
            printHelpAndExit(EXIT_FAILURE);
            break;
        case ERR_BAD_ARG_X:
            fprintf(stderr, COLOR_RED"ERROR, bad argument: %s\n"COLOR_NORMAL, msg);
            printHelpAndExit(EXIT_FAILURE);
            break;
        case ERR_UNKNOWN_ARG_X:
            fprintf(stderr, COLOR_RED"ERROR, unknown argument: %s\n"COLOR_NORMAL, msg);
            printHelpAndExit(EXIT_FAILURE);
            break;
        case ERR_IO_FILE:
            fprintf(stderr, COLOR_RED"ERROR, can't open file: %s\n"COLOR_NORMAL, msg);
            perror(" --> ");
            break;
        case ERR_SYSCALL:
            fprintf(stderr, COLOR_RED"ERROR: %s\n"COLOR_NORMAL, msg);
            perror(" --> ");
            break;
        case ERR_EXEC:
            fprintf(stderr, COLOR_RED"ERROR"COLOR_NORMAL" exec failed doing: %s\n", msg);
            perror(" --> ");
            break;
        default:
            fprintf(stderr, COLOR_RED"ERROR"COLOR_NORMAL" not specified: %s\n", msg);
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