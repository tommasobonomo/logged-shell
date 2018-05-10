#ifndef COMMANDS_H
#define COMMANDS_H

#include "utilities.h"
#include <unistd.h>
#include <sys/types.h>

#define LOG_FORMAT_TXT 0
#define LOG_FORMAT_CSV 1

#define MODE_DISCARD  0
#define MODE_SCREEN   1
#define MODE_LOG      2
#define MODE_FILEAPP  3
#define MODE_FILEOVER 4

/**
 * Struct where a single sub-command is stored
 */
struct SubCommandResult
{
    pid_t ID;
    char *subCommand;
    char parameters;
    //TODO aggiungere statistiche
};

/**
 * Main struct where a full command is stored
 */
struct Command
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
    struct SubCommandResult *subCommandResults[MAX_SUBCOMMANDS];
};

#endif