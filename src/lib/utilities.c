#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include "utilities.h"
#include "../parser/parser.h"
#include "../lib/commands.h"
#include "../lib/syscalls.h"

void printHelpAndExit()
{
    printf("\n###### "TOOL_NAME" ######\n");
    printf("\nGet useful statistics from commands passed as parameters and log them to file\n"
           "or simply output them, together with commands output and errors.\n\n");
    printf("Usage: "TOOL_NAME" [OPTION] \"<command>\"\n\n");
    printf("  -"ARG_LOGPATH", --"ARG_MNEM_LOGPATH"\t\tspecifies the log file path\n");
    printf("  -"ARG_LOGFORMAT", --"ARG_MNEM_LOGFORMAT"\t\tspecifies the format for the log file\n");
    printf("      --"ARG_MNEM_OUTDISCARD"\t\tdiscards command output\n");
    printf("  -"ARG_OUTSCREEN", --"ARG_MNEM_OUTSCREEN"\t\tprints command output to screen instead of creating a log file\n");
    printf("  -"ARG_OUTFILEAPP", --"ARG_MNEM_OUTFILEAPP"\tappends command output to file (requires specifying its path)\n");
    printf("  -"ARG_OUTFILEOVER", --"ARG_MNEM_OUTFILEOVER"\toverwrites output file content (requires specifying its path)\n");
    printf("      --"ARG_MNEM_ERRDISCARD"\t\tdiscards command errors\n");
    printf("  -"ARG_ERRSCREEN", --"ARG_MNEM_ERRSCREEN"\t\tprints command errors to screen instead of creating a log file\n");
    printf("  -"ARG_ERRFILEAPP", --"ARG_MNEM_ERRFILEAPP"\tappends command errors to file (requires specifying its path)\n");
    printf("  -"ARG_ERRFILEOVER", --"ARG_MNEM_ERRFILEOVER"\toverwrites errors file content (requires specifying its path)\n");
    printf("\n");
    printf("  -"ARG_HELP", --"ARG_MNEM_HELP"\t\t\tshows this help and exits\n");
    printf("  -"ARG_VERSION", --"ARG_MNEM_VERSION"\t\t\tprints the current version of this tool and exits\n");

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
