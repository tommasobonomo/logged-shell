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

void getChildrenProcessStats(SubCommandResult *subCommandResult)
{
    struct rusage usage;
    getrusage(RUSAGE_CHILDREN, &usage);

    subCommandResult->cputime = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec;
    subCommandResult->vmressize = usage.ru_maxrss;
    subCommandResult->softPageFaults = usage.ru_minflt;
    subCommandResult->hardPageFaults = usage.ru_majflt;
    subCommandResult->swaps = usage.ru_nswap;
    subCommandResult->signals = usage.ru_nsignals;
    subCommandResult->voluntary_ctxt_switches = usage.ru_nvcsw;
    subCommandResult->nonvoluntary_ctxt_switches = usage.ru_nivcsw;


}

void printStatsC(FILE *fp, Command *cmd)
{
    DEBUG_PRINT("\n######### STATS #########\n");
    DEBUG_PRINT("Command: %s\n", cmd->command);
    int i;
    for (i = 0; i < cmd->n_subCommands; i++)
    {
        printStatsS(fp, cmd->subCommandResults[i]);
    }
}

void printStatsS(FILE *fp, SubCommandResult *subCommandResult)
{
    fprintf(fp, "Subcommand: %s\n", subCommandResult->subCommand);
    fprintf(fp, "  Real elapsed time: %f seconds \n", subCommandResult->totTime);
    fprintf(fp, "  PID: %d\n", subCommandResult->pid);
    fprintf(fp, "  system CPU time used: %ld μs\n", subCommandResult->cputime);
    fprintf(fp, "  maximum resident set size: %ld kB\n", subCommandResult->vmressize);
    fprintf(fp, "  page reclaims (soft page faults): %ld\n", subCommandResult->softPageFaults);
    fprintf(fp, "  page faults (hard page faults): %ld\n", subCommandResult->hardPageFaults);
    fprintf(fp, "  swaps: %ld\n", subCommandResult->swaps);
    fprintf(fp, "  signals received: %ld\n", subCommandResult->signals);
    fprintf(fp, "  voluntary context switches: %ld\n", subCommandResult->voluntary_ctxt_switches);
    fprintf(fp, "  involuntary context switches: %ld\n\n", subCommandResult->nonvoluntary_ctxt_switches);

}

void getProcessStats(pid_t pid, SubCommandResult *subcommand)
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
        switch (i)
        {
            case 3:
            {     // state
                break;
            }
            case 4:
            {     // ppid
                break;
            }
            case 20:
            {    // threads
                break;
            }
            case 23:
            {    // vmsize
                break;
            }
            case 24:
            {    // vmressize
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
