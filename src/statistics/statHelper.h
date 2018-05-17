#ifndef STATHELPER_H
#define STATHELPER_H

#include <stdlib.h>
#include <sys/types.h>
#include "../lib/commands.h"

void getChildrenProcessStats(struct SubCommandResult *subCommandResult);

void getProcessStats(pid_t pid, struct SubCommandResult *subcommand);

void printStatsC(struct Command *cmd);

void printStatsS(struct SubCommandResult *subCommandResult);

#endif