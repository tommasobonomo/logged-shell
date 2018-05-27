#include "./utilities.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include "../parser/parser.h"
#include "../lib/commands.h"
#include "../lib/syscalls.h"
#include "../lib/errors.h"
#include "../daemon/daemon.h"

#define MAX_TERM_LINES 30

/**
 * Little helper for printHelpAndExit to print a string to a new line
 * @param string String to be printed on a new line
 */
void newLine(const char *string)
{
    printf("\n%*c\t\t  %s\n", 15, ' ', string);
}

void printHelpAndExit(int status)
{
    printf("Usage: "TOOL_NAME" [OPTION]... \"SCRIPT\"\n");
    printf("Execute and get useful statistics from SCRIPT, logging them to file.\n\n");

    printf("  -"ARG_OUTFILEAPP", --"ARG_MNEM_OUTFILEAPP"=FILE\tuse append mode when writing command output to FILE\n");
    printf("  -"ARG_ERRFILEAPP", --"ARG_MNEM_ERRFILEAPP"=FILE\tcommand errors will be appended to FILE\n");
    printf("  -"ARG_ERRSCREEN", --"ARG_MNEM_ERRSCREEN"\tprints command errors to screen instead of creating a");
    newLine("log file");
    printf("  -"ARG_LOGFORMAT", --"ARG_MNEM_LOGFORMAT"=FORMAT\tspecifies the FORMAT for the log file, choose from");
    newLine("\"txt\" (default) or \"csv\"");
    printf("      --"ARG_MNEM_ERRDISCARD"\t\tdiscards command errors\n");
    printf("      --"ARG_MNEM_OUTDISCARD"\t\tdiscards command output\n");
    printf("  -"ARG_OUTSCREEN", --"ARG_MNEM_OUTSCREEN"\tprints command output to screen instead of creating a");
    newLine("log file");
    printf("  -"ARG_LOGPATH", --"ARG_MNEM_LOGPATH"=FILE\tspecifies the full path for the log FILE\n");
    printf("  -"ARG_QUIET", --"ARG_MNEM_QUIET"\t\tdo-not-bother-me mode, neither output nor errors will");
    newLine("be displayed");
    printf("  -"ARG_SHOWDEFLOG", --"ARG_MNEM_SHOWDEFLOG"\t\tshow default log file right here in terminal\n");
    printf("  -"ARG_SHOWCUSTLOG", --"ARG_MNEM_SHOWCUSTLOG"=FILE\tshow custom log FILE content\n");
    printf("  -"ARG_OUTFILEOVER", --"ARG_MNEM_OUTFILEOVER"=FILE\tuse overwrite mode when writing command output to FILE\n");
    printf("  -"ARG_ERRFILEOVER", --"ARG_MNEM_ERRFILEOVER"=FILE\tcommand errors will be overwritten in FILE\n");
    printf("      --"ARG_MNEM_HELP"\t\tshows this help and exits\n");
    printf("      --"ARG_MNEM_VERSION"\t\tprints the current version of this tool and exits\n");

    exitAndNotifyDaemon(status);
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

void showLogAndExit(const char *path)
{
    FILE *log_fd;
    log_fd = fopen(path, "r");

    if (log_fd == NULL)
    {
        printf(COLOR_RED"\nLOG PATH:"COLOR_RESET" %s\n\n", path);
        if (strcmp(path, DEFAULT_LOGPATH_TXT) == 0)
        {
            printf("Mmmm, have you already tried executing some commands?\n\n");
        }
        error_fatal(ERR_IO_FILE, path);
    }

    printf(COLOR_GREEN"\nLOG PATH:"COLOR_RESET" %s\n\n", path);

    int lineno = 0;
    char line[MAX_STRING_LENGTH];
    while(fgets(line, MAX_STRING_LENGTH, log_fd) != NULL)
    {
        printf(line);
        lineno++;
    }

    if (lineno > MAX_TERM_LINES)
    {
        printf(COLOR_GREEN"LOG PATH:"COLOR_RESET" %s\n\n", path);
    }

    fclose(log_fd);
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
