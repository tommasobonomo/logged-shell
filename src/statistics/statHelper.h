#ifndef STATHELPER_H
#define STATHELPER_H

#include <stdlib.h>
#include <sys/types.h>
#include "../lib/commands.h"

void getChildrenProcessStats(struct SubCommandResult *subcommand);

void getProcessStats(pid_t pid, struct SubCommandResult *subcommand);

#endif