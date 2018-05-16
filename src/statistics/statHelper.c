#include "./statHelper.h"
#include <sys/resource.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include "../lib/errors.h"
#include "../lib/utilities.h"
#include "../lib/commands.h"

#define PROC_STAT_VALUES_N 52

/*
 * USEFUL:
 * getrusage
 * /usr/bin/time -v "programName"
 * man proc
 * https://www.softprayog.in/tutorials/linux-process-execution-time
 * https://www.linux.com/news/discover-possibilities-proc-directory
 */

void getProcessStats(pid_t pid, struct SubCommandResult *subcommand)
{
    DEBUG_PRINT("### STATISTICS ###\n");

    char statsPath[20];
    sprintf(statsPath, "/proc/%d", pid);

    /* PROCESS STATUS */
    char statFile[20];
    sprintf(statFile, "%s/stat", statsPath);
    FILE *statFileStream = fopen(statFile, "r"); //TODO fare wrapper
    if (statFileStream == NULL) {
        error_fatal(ERR_X, "Failed to open 'stat' file\n");
    }
    char status[1000] = "";
//    char *statsFromStatus[] = {"State", "Pid", "PPid", "VmSize", "VmRSS", "VmSwap", "Threads", "voluntary_ctxt_switches", "nonvoluntary_ctxt_switches"};


    char delimiter = ' ';
    char buf[80];

    for (int i = 1; ! feof(statFileStream) && i <= PROC_STAT_VALUES_N; i++) {
        fscanf(statFileStream, "%s", buf);
        switch (i) {
            case 1:     // pid
                break;
            case 3:     // state
                break;
            case 4:     // ppid
                break;
            case 20:    // threads
                break;
            case 23:    // vmsize
                break;
            case 24:    // vmressize
                break;
            default:
                break;
        }
        //printf("%d: %s\n", i, buf);
    }

    fclose(statFileStream);
    //printf("Process status:\n%s\n", status);


    /* OPENED FILES */
    /*char commandListFD[25];
    char *commandStatFormatter = "stat -c \"File: %N\tdim: %s bytes, owner: %U, modified: %y\" $f";
    sprintf(commandListFD, "cd %s/fd; for f in * \ndo\n%s\ndone", statsPath, commandStatFormatter);
    FILE *fp = popen(commandListFD, "r");
    if (fp == NULL) {
        error_fatal(ERR_X, "Failed to list opened files\n");
    }*/

    /* Read the output a line at a time */
    /*char tmp[100], fds[1000] = "";
    while (fgets(tmp, sizeof(tmp), fp) != NULL) {
        strcat(fds, tmp);
    }

    // close
    pclose(fp);

    printf("Opened files:\n%s\n", fds);*/

    DEBUG_PRINT("\n");
}