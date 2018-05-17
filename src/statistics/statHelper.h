#ifndef STATHELPER_H
#define STATHELPER_H

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include "../lib/commands.h"

void getChildrenProcessStats(struct SubCommandResult *subCommandResult);

void getProcessStats(pid_t pid, struct SubCommandResult *subcommand);

void printStatsC(FILE *fp, struct Command *cmd);

void printStatsS(FILE *fp, struct SubCommandResult *subCommandResult);

#endif