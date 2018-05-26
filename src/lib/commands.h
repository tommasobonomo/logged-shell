#ifndef COMMANDS_H
#define COMMANDS_H

#include "./utilities.h"
#include <unistd.h>
#include <sys/types.h>

#define LOG_FORMAT_TXT 0
#define LOG_FORMAT_CSV 1

#define MODE_DISCARD 0
#define MODE_SCREEN 1
#define MODE_FILEAPP 2
#define MODE_FILEOVER 3

#define NOT_QUIET 0
#define QUIET 1

/**
 * Struct where a single sub-command is stored
 */
typedef struct SubCommandResult
{
    bool executed;
    char subCommand[MAX_STRING_LENGTH];
	// STATS
    pid_t pid;
    pid_t pgid;
    pid_t sid;
    int exitStatus;
    double totRealTime;
    double cputime;
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
    char command[MAX_STRING_LENGTH];
    char log_path[MAX_STRING_LENGTH];
	int log_format;
	int output_mode;
    char output_path[MAX_STRING_LENGTH];
	int error_mode;
    char error_path[MAX_STRING_LENGTH];
	int n_subCommands;
	char username[MAX_STRING_LENGTH];
	__uid_t uid;
	int quiet_mode;
	SubCommandResult subCommandResults[MAX_SUBCOMMANDS];
} Command;

#endif
