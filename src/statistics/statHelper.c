#include "./statHelper.h"
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../lib/errors.h"

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

    /* Open the command for reading. */
    char commandListFD[25];
    char *commandStatFormatter = "stat -c \"File: %N\tdim: %s bytes, owner: %U, modified: %y\" $f";
    sprintf(commandListFD, "cd %s/fd; for f in * \ndo\n%s\ndone", statsPath, commandStatFormatter);
    FILE *fp = popen(commandListFD, "r");
    if (fp == NULL) {
        error_fatal(ERR_X, "Failed to run commandListFD\n");
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