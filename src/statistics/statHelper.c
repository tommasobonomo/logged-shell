#include "./statHelper.h"
#include <sys/resource.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
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
    time_t now = time(NULL);
    struct tm nowFormatted = *localtime(&now);

    fprintf(fp, "############################## STATS ############################\n");
    fprintf(fp, "#\n");
    fprintf(fp, "#      %d-%02d-%02d %02d:%02d:%02d\n", nowFormatted.tm_year + 1900, nowFormatted.tm_mon + 1,
            nowFormatted.tm_mday, nowFormatted.tm_hour, nowFormatted.tm_min, nowFormatted.tm_sec);
    fprintf(fp, "#\n");
    fprintf(fp, "#      whole command: %s\n", cmd->command);
    fprintf(fp, "#      n° subcommand: %d\n", cmd->n_subCommands);
    fprintf(fp, "#\n");
    int i;
    for (i = 0; i < cmd->n_subCommands; i++)
    {
        printStatsS(fp, &cmd->subCommandResults[i]);
    }
    fprintf(fp, "#################################################################\n\n\n");
}

void printStatsS(FILE *fp, SubCommandResult *subCommandResult)
{
    fprintf(fp, "#    ========================================================\n");
    fprintf(fp, "#\n");
    fprintf(fp, "#                 Subcommand: %s\n", subCommandResult->subCommand);
    fprintf(fp, "#\n");
    fprintf(fp, "#                        PID:%14d\n", subCommandResult->pid);
    fprintf(fp, "#                exit status:%14d\n", subCommandResult->exitStatus);
    fprintf(fp, "#               elapsed time:%14f s\n", subCommandResult->totTime);
    fprintf(fp, "#              CPU time used:%14ld μs\n", subCommandResult->cputime);
    fprintf(fp, "#               max ram size:%14ld kB\n", subCommandResult->vmressize);
    fprintf(fp, "#           soft page faults:%14ld\n", subCommandResult->softPageFaults);
    fprintf(fp, "#           hard page faults:%14ld\n", subCommandResult->hardPageFaults);
    fprintf(fp, "#                      swaps:%14ld\n", subCommandResult->swaps);
    fprintf(fp, "#           signals received:%14ld\n", subCommandResult->signals);
    fprintf(fp, "#           signals received:%14ld\n", subCommandResult->voluntary_ctxt_switches);
    fprintf(fp, "#      vol. context switches:%14ld\n", subCommandResult->voluntary_ctxt_switches);
    fprintf(fp, "#      inv. context switches:%14ld\n", subCommandResult->nonvoluntary_ctxt_switches);
    fprintf(fp, "#\n");
}