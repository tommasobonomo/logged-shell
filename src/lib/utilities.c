#include "./utilities.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include "../parser/parser.h"
#include "../lib/commands.h"
#include "../lib/syscalls.h"
#include "../lib/errors.h"

/**
 * Little helper for printHelpAndExit to print a string to a new line
 * @param string String to be printed on a new line
 */
void newLine(const char *string)
{
    printf("\n%*c\t\t  %s\n", 15, ' ', string);
}

void printHelpAndExit()
{
    printf("Usage: "TOOL_NAME" [OPTION]... \"SCRIPT\"\n");
    printf("Execute and get useful statistics from SCRIPT, logging them to a file or simply\nto stdout.\n\n");

    printf("  -"ARG_OUTFILEAPP", --"ARG_MNEM_OUTFILEAPP"\t\tuse append mode when writing command output to file;");
    newLine("you also have to specify its path, use -p to do so");
    printf("  -"ARG_ERRFILEAPP", --"ARG_MNEM_ERRFILEAPP"\t\tuse append mode when writing command errors to file;");
    newLine("you also have to specify its path, use -p to do so");
    printf("  -"ARG_ERRSCREEN", --"ARG_MNEM_ERRSCREEN"\t\tprints command errors to screen instead of creating a");
    newLine("log file");
    printf("  -"ARG_LOGFORMAT", --"ARG_MNEM_LOGFORMAT"=FORMAT\tspecifies the FORMAT for the log file, choose from");
    newLine("\"txt\" (default) or \"csv\"");
    printf("      --"ARG_MNEM_ERRDISCARD"\t\tdiscards command errors\n");
    printf("      --"ARG_MNEM_OUTDISCARD"\t\tdiscards command output\n");
    printf("  -"ARG_OUTSCREEN", --"ARG_MNEM_OUTSCREEN"\t\tprints command output to screen instead of creating a");
    newLine("log file");
    printf("  -"ARG_LOGPATH", --"ARG_MNEM_LOGPATH"=FILEPATH\tspecifies the full path for the log file with FILEPATH\n");
    printf("  -"ARG_OUTFILEOVER", --"ARG_MNEM_OUTFILEOVER"\t\tuse overwrite mode when writing command output to file;");
    newLine("you also have to specify its path, use -p to do so");
    printf("  -"ARG_ERRFILEOVER", --"ARG_MNEM_ERRFILEOVER"\t\tuse overwrite mode when writing command errors to file;");
    newLine("you also have to specify its path, use -p to do so");
    printf("      --"ARG_MNEM_HELP"\t\tshows this help and exits\n");
    printf("      --"ARG_MNEM_VERSION"\t\tprints the current version of this tool and exits\n");

    exitAndNotifyDaemon(EXIT_SUCCESS);
}

void printVersionAndExit()
{
    printf(""TOOL_NAME" "TOOL_VERSION_CODE"\n\n");
    printf("Created in Trento with <3 by:\n");
    printf(" - Tommaso Bonomo\n");
    printf(" - Francesco Cadei\n");
    printf(" - Federico Favotto\n");
    printf(" - Andrea Zanotto\n");

    exitAndNotifyDaemon(EXIT_SUCCESS);
}

bool strStartWith(const char *str, const char *prefix)
{
    return strncmp(str, prefix, strlen(prefix)) == 0 ? true : false;
}

void sanityCheck()
{
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
