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

#define STRING_SIZE 80

/**
 * Struct where a single sub-command is stored
 */
typedef struct SubCommandResult
{
	char subCommand[STRING_SIZE];
	char parameters;
	// STATS
	pid_t pid;
	pid_t ppid; // TODO find it
	double totTime;
	time_t cputime;
	long vmressize;
	long swaps;
	long softPageFaults;
	long hardPageFaults;
	int threads; // TODO thread number
	long signals;
	long voluntary_ctxt_switches;
	long nonvoluntary_ctxt_switches;
} SubCommandResult;

/**
 * Main struct where a full command is stored
 */
typedef struct Command
{
	char *command;
	char *log_path;
	int log_format;
	bool create_log_ifNotExist; //default=true
	int output_mode;
	char *output_path;
	int error_mode;
	char *error_path;
	int n_subCommands;
	SubCommandResult *subCommandResults[MAX_SUBCOMMANDS];
} Command;

#endif
