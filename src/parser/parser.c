#include "parser.h"
#include "../lib/commands.h"
#include "../lib/errors.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define ARG_MEM_LOGPATH "logpath"

/**
 * Add all the default parameters
 * @param cmd the struct in which add the parameters
 * @return the struct with all defaults set
 */
struct Command *addDefault(struct Command *cmd)
{
    //TODO aggiungi tutti i parametri di default
    cmd->command = NULL;
    cmd->log_path = NULL;
    return cmd;
}

void setCommand(struct Command *cmd, char *str_cmd)
{
    if (cmd->command != NULL)
    {
        error_fatal(ERR_X, "command alredy specified");
    }
    else
    {
        cmd->command = str_cmd;
    }
}

void setLogformat(struct Command *cmd, char *format)
{
    if (strcmp(format, "txt") == 0)
    {
        cmd->log_format = LOG_FORMAT_TXT;
    }
    else if (strcmp(format, "csv") == 0)
    {
        cmd->log_format = LOG_FORMAT_CSV;
    }
    else
    {
        error_fatal(ERR_BAD_ARG_X, format);
    }
}

void setLogfile(struct Command *cmd, char *path)
{
    if (cmd->log_path != NULL)
    {
        error_fatal(ERR_X, "log path alredy specified");
    }
    else
    {
        cmd->log_path = path;
    }
}

void setOutputMode(struct Command *cmd, int mode)
{
    cmd->output_mode = mode;
}

void setErrorMode(struct Command *cmd, int mode)
{
    cmd->error_mode = mode;
}

void setOutputPath(struct Command *cmd, char *path)
{
    if (cmd->output_path != NULL)
    {
        error_fatal(ERR_X, "output path alredy specified");
    }
    else
    {
        cmd->output_path = path;
    }
}

void setErrorPath(struct Command *cmd, char *path)
{
    if (cmd->error_path != NULL)
    {
        error_fatal(ERR_X, "error path alredy specified");
    }
    else
    {
        cmd->error_path = path;
    }
}

void setCreate_log_ifNotExist(struct Command *cmd, bool create)
{
    cmd->create_log_ifNotExist = create;
}

/**
 * Move <b>argc</b> to the next parameter if possible
 * If it's not possible, print an error and exit
 * @param currArgc The index of the current argument being inspected
 * @param argc The maximum number of command line arguments
 */
void nextParam(int *currArgc, int argc)
{
    (*currArgc)++;
    if (*currArgc >= argc)
    {
        error_fatal(ERR_BAD_ARGS, NULL);
    }
}

/**
 * Useful function to get a secondaryArg
 * --arg="secondaryArg"
 * --arg "secondaryArg"
 * -a secondaryArg
 * @param argc The maximum number of command line arguments
 * @param argv The array of arguments
 * @param currArgc The index of the current argument being inspected
 * @return The first char of the secondary argument
 */
char *getSecondaryArg(const int argc, char *argv[], int *currArgc)
{
    char *getFromHere;

    getFromHere = strstr(argv[*currArgc], "=");
    if (getFromHere != NULL)
    {
        //"=" found, let's search in this parameter
        getFromHere++;
        if (*getFromHere == '\0')
        {
            error_fatal(ERR_BAD_ARG_X, "log file path");
        }
    }
    else
    {
        //"=" NOT found, let's move to the next parameter
        nextParam(currArgc, argc);
        getFromHere = argv[*currArgc];
    }

    return getFromHere;
}

struct Command *parseCommand(int argc, char *argv[])
{
    struct Command *result = malloc(sizeof(struct Command));
    addDefault(result);
    DEBUG_PRINT("result allocato, default added\n\n");

    for (int currArgc = 1; currArgc < argc; currArgc++)
    {
        if (argv[currArgc][0] == '-')
        {
            if (argv[currArgc][1] != '-')
            {
                //NORMAL ARG

            }
            else
            {
                //MNEMONIC ARG
                //TODO fare per tutti ARG_NOR_... e ARG_MEM_...
                if (strStartWith(&argv[currArgc][2], ARG_MEM_LOGPATH))
                {
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setLogfile(result, secondaryArg);
                    DEBUG_PRINT("path logfile: %s\n", result->log_path);
                }
                else if (strStartWith(&argv[currArgc][2], "format"))
                {
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setLogformat(result, secondaryArg);
                    DEBUG_PRINT("format: %d\n", result->log_format);
                }
                else if (strStartWith(&argv[currArgc][2], "discardout"))
                {
                    setOutputMode(result, MODE_DISCARD);
                    DEBUG_PRINT("outputmode: %d\n", result->output_mode);
                }
                else if (strStartWith(&argv[currArgc][2], "discarderr"))
                {
                    setErrorMode(result, MODE_DISCARD);
                    DEBUG_PRINT("errormode: %d\n", result->error_mode);
                }
                else if (strStartWith(&argv[currArgc][2], "printout"))
                {
                    setOutputMode(result, MODE_SCREEN);
                    DEBUG_PRINT("outputmode: %d\n", result->output_mode);
                }
                else if (strStartWith(&argv[currArgc][2], "printerr"))
                {
                    setErrorMode(result, MODE_SCREEN);
                    DEBUG_PRINT("errormode: %d\n", result->error_mode);
                }
                else if (strStartWith(&argv[currArgc][2], "logout")) //TODO not the best name in the world
                {
                    setOutputMode(result, MODE_LOG);
                    DEBUG_PRINT("outputmode: %d\n", result->output_mode);
                }
                else if (strStartWith(&argv[currArgc][2], "logerr"))
                {
                    setErrorMode(result, MODE_LOG);
                    DEBUG_PRINT("errormode: %d\n", result->error_mode);
                }
                else if (strStartWith(&argv[currArgc][2], "fileappout"))
                {
                    setOutputMode(result, MODE_FILEAPP);
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setOutputPath(result, secondaryArg);
                    DEBUG_PRINT("outputmode: %d\n", result->output_mode);
                    DEBUG_PRINT("outputpath: %s\n", result->output_path);
                }
                else if (strStartWith(&argv[currArgc][2], "fileapperr"))
                {
                    setErrorMode(result, MODE_FILEAPP);
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setErrorPath(result, secondaryArg);
                    DEBUG_PRINT("errormode: %d\n", result->error_mode);
                    DEBUG_PRINT("errorpath: %s\n", result->error_path);
                }
                else if (strStartWith(&argv[currArgc][2], "fileoverout"))
                {
                    setOutputMode(result, MODE_FILEOVER);
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setOutputPath(result, secondaryArg);
                    DEBUG_PRINT("outputmode: %d\n", result->output_mode);
                    DEBUG_PRINT("outputpath: %s\n", result->output_path);
                }
                else if (strStartWith(&argv[currArgc][2], "fileovererr"))
                {
                    setErrorMode(result, MODE_FILEOVER);
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setErrorPath(result, secondaryArg);
                    DEBUG_PRINT("errormode: %d\n", result->error_mode);
                    DEBUG_PRINT("errorpath: %s\n", result->error_path);
                }

                else if (strStartWith(&argv[currArgc][2], "dontcreate"))
                {
                    setCreate_log_ifNotExist(result, false);
                    DEBUG_PRINT("create_log_ifNotExist: %d\n", result->create_log_ifNotExist);
                }
                else
                {
                    error_fatal(ERR_UNKNOWN_ARG_X, argv[currArgc]);
                }
            }
        }
        else
        {
            setCommand(result, argv[currArgc]);
            DEBUG_PRINT("command: %s\n", result->command);
        }
    }

    return result;
}

/**
 * Spot a command delimiter
 * @param c Character to analyse
 * @return True when <b>c</b> is a command delimiter, false otherwise
 */
bool isspecial(char c)
{
    switch (c)
    {
        case '|':
        case ';':
        case '<':
        case '>':
        case '&':
            return true;
        default:
            return false;
    }
}

void getNextSubCommand(char *str, char **start, char **end)
{
    if (str == NULL)
    {
        error_fatal(ERR_BAD_ARG_X, "Command to execute not specified");
    }

    bool exit = false;
    *start = NULL;
    *end = NULL;

    for (int i = 0; !exit; i++)
    {
        if (str[i] == '\0')
        {
            exit = true;
        }
        else if (isspecial(str[i]))
        {
            if (*start == NULL) //trovato primo carattere
            {
                *start = *end = &str[i];
            }
            exit = true;
        }
        else
        {
            if (*start == NULL && !isspace(str[i])) //trovato primo carattere
            {
                *start = &str[i];
            }
            else if (!isspace(str[i]))//trovato ulteriore carattere
            {
                *end = &str[i];
            }
        }
    }
}