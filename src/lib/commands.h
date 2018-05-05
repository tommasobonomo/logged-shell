#ifndef COMMANDS_H
#define COMMANDS_H

#include "utilities.h"

#define LOG_FORMAT_TXT 0
#define LOG_FORMAT_CSV 1

#define MODE_DISCARD  0
#define MODE_SCREEN   1
#define MODE_LOG      2
#define MODE_FILEAPP  3
#define MODE_FILEOVER 4

struct SubCommand
{
    char subCommand[CMD_STRING_LENGHT_MAX + 1];
    char parameters[CMD_STRING_LENGHT_MAX + 1];
    //TODO aggiungere statistiche
};

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
    struct SubCommand *subCommands[];
};

#endif