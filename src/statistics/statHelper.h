#ifndef STATHELPER_H
#define STATHELPER_H

#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include "../lib/commands.h"

/**
 * Gets statistics from getrusage and save them to subCommandResult
 * @param subCommandResult where to save stats
 * @param usage getrusage struct
 */
void saveProcessStats(SubCommandResult *subCommandResult, struct rusage *usage);

/**
 * Print formatted statistics to file (wrapper)
 * @param fp where to save stats
 * @param cmd command struct
 */
void printStatsCommand(FILE *fp, Command *cmd);

/**
 * TXT format file printer (specific command)
 * @param fp where to save stats
 * @param subCommandResult subcommand struct
 */
void printStatsSubCommandTxt(FILE *fp, SubCommandResult *subCommandResult);

/**
 * CSV format file printer (specific command)
 * @param fp where to save stats
 * @param subCommandResult subcommand struct
 */
void printStatsSubCommandCsv(FILE *fp, Command *cmd, SubCommandResult *subCommandResult);

#endif
