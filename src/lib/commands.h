#ifndef COMMANDS_H
#define COMMANDS_H

#include <unistd.h>
#include <sys/types.h>
#include "utilities.h"

#define LOG_FORMAT_TXT 0
#define LOG_FORMAT_CSV 1

#define MODE_DISCARD 0
#define MODE_SCREEN 1
#define MODE_LOG 2
#define MODE_FILEAPP 3
#define MODE_FILEOVER 4

/**
 * Struct where a single sub-command is stored
 */
typedef struct SubCommandResult
{
    bool executed;
    char subCommand[MAX_STRING_LENGHT];
	// STATS
	pid_t pid;
    int exitStatus;
    double totRealTime;
	time_t cputime;
	long vmressize;
	long softPageFaults;
    long hardPageFaults;
    long swaps;
    long signals;
	long voluntary_ctxt_switches;
	long nonvoluntary_ctxt_switches;
} SubCommandResult;

/**
 * Main struct where a full command is stored
 */
typedef struct Command
{
    char command[MAX_STRING_LENGHT];
    char log_path[MAX_STRING_LENGHT];
	int log_format;
    bool create_log_ifNotExist;
	int output_mode;
    char output_path[MAX_STRING_LENGHT];
	int error_mode;
    char error_path[MAX_STRING_LENGHT];
	int n_subCommands;
    SubCommandResult subCommandResults[MAX_SUBCOMMANDS];
} Command;

void commandCopy(Command *dst, Command *src);

void subCommandCopy(SubCommandResult *dst, SubCommandResult *src);

#endif
