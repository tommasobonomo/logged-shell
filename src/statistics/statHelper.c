#include "./statHelper.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <time.h>
#include "../lib/errors.h"
#include "../lib/utilities.h"
#include "../lib/commands.h"


void saveProcessStats(SubCommandResult *subCommandResult, struct rusage *usage)
{
    subCommandResult->cputime = usage->ru_stime.tv_sec + usage->ru_stime.tv_usec;
    subCommandResult->vmressize = usage->ru_maxrss;
    subCommandResult->softPageFaults = usage->ru_minflt;
    subCommandResult->hardPageFaults = usage->ru_majflt;
    subCommandResult->swaps = usage->ru_nswap;
    subCommandResult->signals = usage->ru_nsignals;
    subCommandResult->voluntary_ctxt_switches = usage->ru_nvcsw;
    subCommandResult->nonvoluntary_ctxt_switches = usage->ru_nivcsw;
}

void printStatsCommand(FILE *fp, Command *cmd)
{
    time_t now = time(NULL);
    struct tm nowFormatted = *localtime(&now);

    if (cmd->log_format == LOG_FORMAT_TXT)
    {
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
            printStatsSubCommandTxt(fp, &cmd->subCommandResults[i]);
        }
        fprintf(fp, "#################################################################\n\n\n");
    }
    else if (cmd->log_format == LOG_FORMAT_CSV)
    {
        fprintf(fp, "\"Command log time\",\"whole command\",\"n° subcommand\",Subcommand,PID,\"exit status\",\"elapsed time\","
                    "\"CPU time used\",\"max ram size\",\"soft page faults\",\"hard page faults\",swaps,\"signals received\","
                    "\"vol. context switches\",\"inv. context switches\"\n");
        int i;
        for (i = 0; i < cmd->n_subCommands; i++)
        {
            fprintf(fp, "\"%d-%02d-%02d %02d:%02d:%02d\",", nowFormatted.tm_year + 1900, nowFormatted.tm_mon + 1,
                    nowFormatted.tm_mday, nowFormatted.tm_hour, nowFormatted.tm_min, nowFormatted.tm_sec);
            printStatsSubCommandCsv(fp, cmd, &cmd->subCommandResults[i]);
        }
    }
}

void printStatsSubCommandTxt(FILE *fp, SubCommandResult *subCommandResult)
{
    fprintf(fp, "#    ========================================================\n");
    fprintf(fp, "#\n");
    if (subCommandResult->executed)
    {
        fprintf(fp, "#                 Subcommand: %s\n", subCommandResult->subCommand);
        fprintf(fp, "#\n");
        fprintf(fp, "#                        PID:%14d\n", subCommandResult->pid);
        fprintf(fp, "#                exit status:%14d\n", subCommandResult->exitStatus);
        fprintf(fp, "#               elapsed time:%14f s\n", subCommandResult->totRealTime);
        fprintf(fp, "#              CPU time used:%14ld μs\n", subCommandResult->cputime);
        fprintf(fp, "#               max ram size:%14ld kB\n", subCommandResult->vmressize);
        fprintf(fp, "#           soft page faults:%14ld\n", subCommandResult->softPageFaults);
        fprintf(fp, "#           hard page faults:%14ld\n", subCommandResult->hardPageFaults);
        fprintf(fp, "#                      swaps:%14ld\n", subCommandResult->swaps);
        fprintf(fp, "#           signals received:%14ld\n", subCommandResult->signals);
        fprintf(fp, "#      vol. context switches:%14ld\n", subCommandResult->voluntary_ctxt_switches);
        fprintf(fp, "#      inv. context switches:%14ld\n", subCommandResult->nonvoluntary_ctxt_switches);
    }
    else
    {
        fprintf(fp, "#                 Subcommand: %s [SKIPPED]\n", subCommandResult->subCommand);
    }
    fprintf(fp, "#\n");
}

void printStatsSubCommandCsv(FILE *fp, Command *cmd, SubCommandResult *subCommandResult)
{
    fprintf(fp, "\"%s\",", cmd->command);
    fprintf(fp, "%d,", cmd->n_subCommands);
    fprintf(fp, "\"%s\",", subCommandResult->subCommand);
    fprintf(fp, "%d,", subCommandResult->pid);
    fprintf(fp, "%d,", subCommandResult->exitStatus);
    fprintf(fp, "\"%f s\",", subCommandResult->totRealTime);
    fprintf(fp, "\"%ld μs\",", subCommandResult->cputime);
    fprintf(fp, "\"%ld kB\",", subCommandResult->vmressize);
    fprintf(fp, "%ld,", subCommandResult->softPageFaults);
    fprintf(fp, "%ld,", subCommandResult->hardPageFaults);
    fprintf(fp, "%ld,", subCommandResult->swaps);
    fprintf(fp, "%ld,", subCommandResult->signals);
    fprintf(fp, "%ld,", subCommandResult->voluntary_ctxt_switches);
    fprintf(fp, "%ld\n", subCommandResult->nonvoluntary_ctxt_switches);
}