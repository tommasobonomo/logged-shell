#include "commands.h"
#include <string.h>

void commandCopy(Command *dst, Command *src)
{
    strcpy(dst->command, src->command);
    strcpy(dst->log_path, src->log_path);
    dst->log_format = src->log_format;
    dst->create_log_ifNotExist = src->create_log_ifNotExist;
    dst->output_mode = src->output_mode;
    strcpy(dst->output_path, src->output_path);
    dst->error_mode = src->error_mode;
    strcpy(dst->error_path, src->error_path);
    dst->n_subCommands = src->n_subCommands;

    int i;
    for (i = 0; i < src->n_subCommands; i++)
    {
        subCommandCopy(&dst->subCommandResults[i], &src->subCommandResults[i]);
    }
}

void subCommandCopy(SubCommandResult *dst, SubCommandResult *src)
{
    strcpy(dst->subCommand, src->subCommand);
    dst->parameters = src->parameters;
    dst->pid = src->pid;
    dst->ppid = src->ppid;
    dst->totTime = src->totTime;
    dst->cputime = src->cputime;
    dst->vmressize = src->vmressize;
    dst->swaps = src->swaps;
    dst->softPageFaults = src->softPageFaults;
    dst->hardPageFaults = src->hardPageFaults;
    dst->threads = src->threads;
    dst->signals = src->signals;
    dst->voluntary_ctxt_switches = src->voluntary_ctxt_switches;
    dst->nonvoluntary_ctxt_switches = src->nonvoluntary_ctxt_switches;
}