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
            fprintf(stderr, ANSI_COLOR_RED"ERROR: %s\n"ANSI_COLOR_RESET, msg);
            break;
        case ERR_BAD_ARGS:
            fprintf(stderr, ANSI_COLOR_RED"ERROR: Bad arguments\n"ANSI_COLOR_RESET);
            printHelpAndExit(EXIT_FAILURE);
            break;
        case ERR_BAD_ARG_X:
            fprintf(stderr, ANSI_COLOR_RED"ERROR, bad argument: %s\n"ANSI_COLOR_RESET, msg);
            printHelpAndExit(EXIT_FAILURE);
            break;
        case ERR_UNKNOWN_ARG_X:
            fprintf(stderr, ANSI_COLOR_RED"ERROR, unknown argument: %s\n"ANSI_COLOR_RESET, msg);
            printHelpAndExit(EXIT_FAILURE);
            break;
        case ERR_IO_FILE:
            fprintf(stderr, ANSI_COLOR_RED"ERROR, can't open file: %s\n"ANSI_COLOR_RESET, msg);
            perror(" --> ");
            break;
        case ERR_SYSCALL:
            fprintf(stderr, ANSI_COLOR_RED"ERROR: %s\n"ANSI_COLOR_RESET, msg);
            perror(" --> ");
            break;
        case ERR_EXEC:
            fprintf(stderr, ANSI_COLOR_RED"ERROR exec failed doing: %s\n"ANSI_COLOR_RESET, msg);
            perror(" --> ");
            break;
        default:
            fprintf(stderr, ANSI_COLOR_RED"ERROR not specified: %s\n"ANSI_COLOR_RESET, msg);
            break;
    }

    exitAndNotifyDaemon(id);
}

void error_warning(int id, char const *msg)
{
    switch (id)
    {
        case ERR_SYSCALL:
            fprintf(stderr, ANSI_COLOR_YELLOW"WARNING: %s\n"ANSI_COLOR_RESET, msg);
            perror(" --> ");
            break;
        case ERR_SIGNAL:
            fprintf(stderr, ANSI_COLOR_YELLOW"WARNING, can't catch signal: %s\n"ANSI_COLOR_RESET, msg);
            perror(" --> ");
            break;
        default:
            fprintf(stderr, ANSI_COLOR_YELLOW"WARNING not specified: %s\n"ANSI_COLOR_RESET, msg);
            break;
    }
}