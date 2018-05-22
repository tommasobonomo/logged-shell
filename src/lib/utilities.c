#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include "utilities.h"
#include "../parser/parser.h"
#include "../lib/commands.h"
#include "../lib/syscalls.h"

void printHelp()
{
    printf("\n###### XLOGGER ######\n");
    printf("\nGet useful statistics from commands passed as parameters and log them to file\n"
           "or simply output them, together with commands output and errors.\n\n");
    printf("Usage: xlogger [OPTION] \"<command>\"\n\n");
    printf("  -p, --%s\t\t\tspecifies the log file path\n", ARG_MEM_LOGPATH);
    printf("  -f, --%s\t\t\tspecifies the format for the log file\n", ARG_MEM_LOGFORMAT);
    printf("      --%s\t\tdiscards command output\n", ARG_MEM_OUTDISCARD);
    printf("  -o, --%s\t\tprints command output to screen instead of creating a log file\n", ARG_MEM_OUTSCEEN);
    printf("  -O, --%s\t\t\tputs command output to a log file (requires specifying its path)\n", ARG_MEM_OUTLOG);
    printf("  -a, --%s\t\tappends command output to file (requires specifying its path)\n", ARG_MEM_OUTFILEAPP);
    printf("  -x, --%s\t\toverwrites output file content (requires specifying its path)\n", ARG_MEM_OUTFILEOVER);
    printf("      --%s\t\tdiscards command errors\n", ARG_MEM_ERRDISCARD);
    printf("  -e, --%s\t\tprints command errors to screen instead of creating a log file\n", ARG_MEM_ERRSCEEN);
    printf("  -E, --%s\t\t\tputs command errors to a log file (requires specifying its path)\n", ARG_MEM_ERRLOG);
    printf("  -A, --%s\t\tappends command errors to file (requires specifying its path)\n", ARG_MEM_ERRFILEAPP);
    printf("  -X, --%s\t\toverwrites errors file content (requires specifying its path)\n", ARG_MEM_ERRFILEOVER);
    printf("  -n, --%s\t\t\tdon't create command log\n", ARG_MEM_DONTCREATELOG);
    printf("\n");
    printf("  -h, --%s\t\t\tshows this help and exits\n", ARG_MEM_HELP);
    printf("  -v, --%s\t\t\tprints the current version of this tool and exits\n", ARG_MEM_VERSION);
    printf("\nCreated in Trento with <3 by:\n");
    printf(" - Tommaso Bonomo\n");
    printf(" - Francesco Cadei\n");
    printf(" - Federico Favotto\n");
    printf(" - Andrea Zanotto\n");
    printf("\n#####################\n");
}

bool strStartWith(const char *str, const char *prefix)
{
    return strncmp(str, prefix, strlen(prefix)) == 0 ? true : false;
}

void sanityCheck()
{
    if (PIPE_BUF < sizeof(SubCommandResult))
    {
        fprintf(stderr, "FATAL ERROR!!!\n");
        fprintf(stderr, "PIPE_BUF max size: %d\n", PIPE_BUF);
        fprintf(stderr, "Struct SubCommandResult size: %d\n", (int) sizeof(SubCommandResult));
        exitAndNotifyDaemon(EXIT_FAILURE);
    }
    DEBUG_PRINT("PIPE_BUF max size: %d\n", PIPE_BUF);
    DEBUG_PRINT("Struct SubCommandResult size: %d\n", (int) sizeof(SubCommandResult));

    FILE *msgmaxFd = w_fopen("/proc/sys/kernel/msgmax", "r");
    unsigned int msgmax;
    fscanf(msgmaxFd, "%d", &msgmax);
    fclose(msgmaxFd);

    if (msgmax < sizeof(Command))
    {
        fprintf(stderr, "FATAL ERROR!!!\n");
        fprintf(stderr, "MSGMAX max size: %d\n", msgmax);
        fprintf(stderr, "Struct Command size: %d\n", (int) sizeof(Command));
        exitAndNotifyDaemon(EXIT_FAILURE);
    }
    DEBUG_PRINT("MSGMAX max size: %d\n", msgmax);
    DEBUG_PRINT("Struct Command size: %d\n", (int) sizeof(Command));
}