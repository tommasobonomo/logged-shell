#ifndef STATHELPER_H
#define STATHELPER_H

#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include "../lib/commands.h"

void saveProcessStats(SubCommandResult *subCommandResult, struct rusage *usage);

void printStatsCommand(FILE *fp, Command *cmd);

void printStatsSubCommandTxt(FILE *fp, SubCommandResult *subCommandResult);

void printStatsSubCommandCsv(FILE *fp, Command *cmd, SubCommandResult *subCommandResult);

#endif
