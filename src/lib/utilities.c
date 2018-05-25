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
    printf("\n###### "TOOL_NAME" ######\n");
    printf("\nGet useful statistics from commands passed as parameters and log them to file\n"
           "or simply output them, together with commands output and errors.\n\n");
    printf("Usage: "TOOL_NAME" [OPTION] \"<command>\"\n\n");
    printf("  -"ARG_LOGPATH", --"ARG_MEM_LOGPATH"\t\tspecifies the log file path\n");
    printf("  -"ARG_LOGFORMAT", --"ARG_MEM_LOGFORMAT"\t\tspecifies the format for the log file\n");
    printf("      --"ARG_MEM_OUTDISCARD"\t\tdiscards command output\n");
    printf("  -"ARG_OUTSCREEN", --"ARG_MEM_OUTSCREEN"\t\tprints command output to screen instead of creating a log file\n");
    printf("  -"ARG_OUTLOG", --"ARG_MEM_OUTLOG"\tputs command output to a log file (requires specifying its path)\n");
    printf("  -"ARG_OUTFILEAPP", --"ARG_MEM_OUTFILEAPP"\tappends command output to file (requires specifying its path)\n");
    printf("  -"ARG_OUTFILEOVER", --"ARG_MEM_OUTFILEOVER"\toverwrites output file content (requires specifying its path)\n");
    printf("      --"ARG_MEM_ERRDISCARD"\t\tdiscards command errors\n");
    printf("  -"ARG_ERRSCREEN", --"ARG_MEM_ERRSCREEN"\t\tprints command errors to screen instead of creating a log file\n");
    printf("  -"ARG_ERRLOG", --"ARG_MEM_ERRLOG"\tputs command errors to a log file (requires specifying its path)\n");
    printf("  -"ARG_ERRFILEAPP", --"ARG_MEM_ERRFILEAPP"\tappends command errors to file (requires specifying its path)\n");
    printf("  -"ARG_ERRFILEOVER", --"ARG_MEM_ERRFILEOVER"\toverwrites errors file content (requires specifying its path)\n");
    printf("  -"ARG_DONTCREATELOG", --"ARG_MEM_DONTCREATELOG"\t\tdon't create command log\n");
    printf("\n");

    //TODO documentation resource

    // printf("Created in Trento with <3 by:\n");
    // printf(" - Tommaso Bonomo\n");
    // printf(" - Francesco Cadei\n");
    // printf(" - Federico Favotto\n");
    // printf(" - Andrea Zanotto\n");
    exitAndNotifyDaemon(EXIT_SUCCESS);
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
    DEBUG_PRINT("\n");
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
