#include "./statHelper.h"
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../lib/errors.h"
#include "../lib/utilities.h"

/*
 * USEFUL:
 * getrusage
 * /usr/bin/time -v "programName"
 * man proc
 * https://www.softprayog.in/tutorials/linux-process-execution-time
 * https://www.linux.com/news/discover-possibilities-proc-directory
 */

void getCurrProcessStats()
{
    printf("\n### STATISTICS ###\n\n");

    char statsPath[20];
    sprintf(statsPath, "/proc/%d", getpid());

    /* PROCESS STATUS */
    char statusFile[20];
    sprintf(statusFile, "%s/status", statsPath);
    FILE *statusfp = fopen(statusFile, "r");
    if (statusfp == NULL) {
        error_fatal(ERR_X, "Failed to open 'status' file\n");
    }
    char status[1000] = "";
    char *statsFromStatus[] = {"State", "Pid", "PPid", "VmSize", "VmRSS", "VmSwap", "Threads", "voluntary_ctxt_switches", "nonvoluntary_ctxt_switches"};
    int countStatsFromStatus = sizeof(statsFromStatus) / sizeof(statsFromStatus[0]);
    for (int i = 0; i < countStatsFromStatus; i++) {
        char grep[50];
        sprintf(grep, "grep -m 1 %s: %s/status", statsFromStatus[i], statsPath);
        FILE *fp = popen(grep, "r");
        if (fp == NULL) {
            error_fatal(ERR_X, "Failed to list opened files\n");
        }
        char line[50];
        fgets(line, sizeof(line), fp);
        strcat(status, line);
        pclose(fp);
    }
    fclose(statusfp);
    printf("Process status:\n%s\n", status);


    /* OPENED FILES */
    char commandListFD[25];
    char *commandStatFormatter = "stat -c \"File: %N\tdim: %s bytes, owner: %U, modified: %y\" $f";
    sprintf(commandListFD, "cd %s/fd; for f in * \ndo\n%s\ndone", statsPath, commandStatFormatter);
    FILE *fp = popen(commandListFD, "r");
    if (fp == NULL) {
        error_fatal(ERR_X, "Failed to list opened files\n");
    }

    /* Read the output a line at a time */
    char tmp[4], fds[1000] = "";
    while (fgets(tmp, sizeof(tmp), fp) != NULL) {
        strcat(fds, tmp);
    }

    /* close */
    pclose(fp);

    printf("Opened files:\n%s\n", fds);
}