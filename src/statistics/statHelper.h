#ifndef STATHELPER_H
#define STATHELPER_H

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include "../lib/commands.h"

void getChildrenProcessStats(SubCommandResult *subCommandResult);

void getProcessStats(pid_t pid, SubCommandResult *subcommand);

void printStatsC(FILE *fp, Command *cmd);

void printStatsS(FILE *fp, SubCommandResult *subCommandResult);

#endif
