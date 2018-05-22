#ifndef STATHELPER_H
#define STATHELPER_H

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include "../lib/commands.h"

void getChildrenProcessStats(SubCommandResult *subCommandResult);

void printStatsCommand(FILE *fp, Command *cmd);

void printStatsSubCommandTxt(FILE *fp, SubCommandResult *subCommandResult);

void printStatsSubCommandCsv(FILE *fp, Command *cmd, SubCommandResult *subCommandResult);

#endif
