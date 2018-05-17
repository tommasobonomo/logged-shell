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

void getChildrenProcessStats(struct SubCommandResult *subcommand)
{
    struct rusage usage;
    getrusage(RUSAGE_CHILDREN, &usage);
    DEBUG_PRINT("###### STATS (getrusage) ######\n");

    subcommand->cputime = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec;
    DEBUG_PRINT("  system CPU time used: %ld μs\n", subcommand->cputime);

    subcommand->vmressize = usage.ru_maxrss;
    DEBUG_PRINT("  maximum resident set size: %ld kB\n", subcommand->vmressize);

    subcommand->softPageFaults = usage.ru_minflt;
    DEBUG_PRINT("  page reclaims (soft page faults): %ld\n", subcommand->softPageFaults);

    subcommand->hardPageFaults = usage.ru_majflt;
    DEBUG_PRINT("  page faults (hard page faults): %ld\n", subcommand->hardPageFaults);

    subcommand->swaps = usage.ru_nswap;
    DEBUG_PRINT("  swaps: %ld\n", subcommand->swaps);

    subcommand->signals = usage.ru_nsignals;
    DEBUG_PRINT("  signals received: %ld\n", subcommand->signals);

    subcommand->voluntary_ctxt_switches = usage.ru_nvcsw;
    DEBUG_PRINT("  voluntary context switches: %ld\n", subcommand->voluntary_ctxt_switches);

    subcommand->nonvoluntary_ctxt_switches = usage.ru_nivcsw;
    DEBUG_PRINT("  involuntary context switches: %ld\n", subcommand->nonvoluntary_ctxt_switches);

    DEBUG_PRINT("###############################\n");
}

void getProcessStats(pid_t pid, struct SubCommandResult *subcommand)
{
    DEBUG_PRINT("###### STATS (proc) ######\n");

    char statsPath[20];
    sprintf(statsPath, "/proc/%d", pid);

    /* PROCESS STATUS */
    char statFile[20];
    sprintf(statFile, "%s/stat", statsPath);
    FILE *statFileStream = fopen(statFile, "r"); //TODO fare wrapper
    if (statFileStream == NULL)
    {
        error_fatal(ERR_X, "Failed to open 'stat' file\n");
    }
//    char *statsFromStatus[] = {"State", "Pid", "PPid", "VmSize", "VmRSS", "VmSwap", "Threads", "voluntary_ctxt_switches", "nonvoluntary_ctxt_switches"};

    char buf[80];

    int i;
    for (i = 1; !feof(statFileStream) && i <= PROC_STAT_VALUES_N; i++)
    {
        fscanf(statFileStream, "%s", buf);
        switch (i) {
            case 3: {     // state
                break;
            }
            case 4: {     // ppid
                break;
            }
            case 20: {    // threads
                break;
            }
            case 23: {    // vmsize
                break;
            }
            case 24: {    // vmressize
                printf("  ressize: %s kB\n", buf);
                break;
            }
            default:
                break;
        }
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

    DEBUG_PRINT("##########################\n");
}