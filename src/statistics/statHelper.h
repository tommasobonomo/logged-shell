#ifndef STATHELPER_H
#define STATHELPER_H

#include <stdlib.h>
#include <sys/types.h>
#include "../lib/commands.h"

void getChildrenProcessStats();

void getProcessStats(pid_t pid, struct SubCommandResult *subcommand);

#endif