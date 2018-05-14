#include "parser.h"
#include "../lib/commands.h"
#include "../lib/errors.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h> //TODO controllare se si può mettere

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

    cmd->n_subCommands = 0;
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
    DEBUG_PRINT("Command allocato, default added\n\n");

    for (int currArgc = 1; currArgc < argc; currArgc++)
    {
        if (argv[currArgc][0] == '-')
        {
            if (argv[currArgc][1] != '-')
            {
                //NORMAL ARG
                if (false)
                {
                }
                else
                {
                    error_fatal(ERR_UNKNOWN_ARG_X, argv[currArgc]);
                }
            }
            else
            {
                //MNEMONIC ARG
                if (strStartWith(&argv[currArgc][2], ARG_MEM_LOGPATH))
                {
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setLogfile(result, secondaryArg);
                    DEBUG_PRINT("path logfile: %s\n", result->log_path);
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MEM_LOGFORMAT))
                {
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setLogformat(result, secondaryArg);
                    DEBUG_PRINT("format: %d\n", result->log_format);
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MEM_OUTDISCARD))
                {
                    setOutputMode(result, MODE_DISCARD);
                    DEBUG_PRINT("outputmode: %d\n", result->output_mode);
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MEM_ERRDISCARD))
                {
                    setErrorMode(result, MODE_DISCARD);
                    DEBUG_PRINT("errormode: %d\n", result->error_mode);
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MEM_OUTSCEEN))
                {
                    setOutputMode(result, MODE_SCREEN);
                    DEBUG_PRINT("outputmode: %d\n", result->output_mode);
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MEM_ERRSCEEN))
                {
                    setErrorMode(result, MODE_SCREEN);
                    DEBUG_PRINT("errormode: %d\n", result->error_mode);
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MEM_OUTLOG)) //TODO not the best name in the world
                {
                    setOutputMode(result, MODE_LOG);
                    DEBUG_PRINT("outputmode: %d\n", result->output_mode);
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MEM_ERRLOG))
                {
                    setErrorMode(result, MODE_LOG);
                    DEBUG_PRINT("errormode: %d\n", result->error_mode);
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MEM_OUTFILEAPP))
                {
                    setOutputMode(result, MODE_FILEAPP);
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setOutputPath(result, secondaryArg);
                    DEBUG_PRINT("outputmode: %d\n", result->output_mode);
                    DEBUG_PRINT("outputpath: %s\n", result->output_path);
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MEM_ERRFILEAPP))
                {
                    setErrorMode(result, MODE_FILEAPP);
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setErrorPath(result, secondaryArg);
                    DEBUG_PRINT("errormode: %d\n", result->error_mode);
                    DEBUG_PRINT("errorpath: %s\n", result->error_path);
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MEM_OUTFILEOVER))
                {
                    setOutputMode(result, MODE_FILEOVER);
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setOutputPath(result, secondaryArg);
                    DEBUG_PRINT("outputmode: %d\n", result->output_mode);
                    DEBUG_PRINT("outputpath: %s\n", result->output_path);
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MEM_ERRFILEOVER))
                {
                    setErrorMode(result, MODE_FILEOVER);
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setErrorPath(result, secondaryArg);
                    DEBUG_PRINT("errormode: %d\n", result->error_mode);
                    DEBUG_PRINT("errorpath: %s\n", result->error_path);
                }

                else if (strStartWith(&argv[currArgc][2], ARG_MEM_DONTCREATELOG))
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
    bool precSpecial = false;
    *start = NULL;
    *end = NULL;

    for (int i = 0; !exit; i++)
    {
        if (str[i] == '\0')
        {
            exit = true;
        }
        else
        {
            if (*start == NULL && str[i] == '&' && isdigit(str[i + 1])) //e.g. &1
            {
                *start = &str[i];
                *end = &str[i + 1];
                exit = true;
            }
            else if (isdigit(str[i]) && str[i + 1] == '>') //e.g. 2>
            {
                if (*start == NULL) //first char, take the number only
                {
                    *start = *end = &str[i];
                    exit = true;
                }
                else //it's not the first so terminate current
                {
                    exit = true;
                }
            }
            else if (isspecial(str[i]))
            {
                if (str[i] == '>' && str[i + 1] == '&') //e.g. >&2
                {
                    if (*start == NULL) //first char, take the > only
                    {
                        *start = *end = &str[i];
                        exit = true;
                    }
                    else
                    {
                        exit = true;
                    }
                }
                else if (precSpecial == true) //go on, we are between some specials char
                {
                    *end = &str[i];
                }
                else
                {
                    if (*start == NULL) //first special char
                    {
                        *start = *end = &str[i];
                    }
                    else //it's not the first so terminate current //e.g. abcd;
                        exit = true;
                }

                precSpecial = true;
            }
            else if (precSpecial == true) //special sequence ended
            {
                exit = true;
            }
            else
            {
                if (*start == NULL && !isspace(str[i])) //first text char found (save only non-space chars)
                {
                    *start = &str[i];
                }
                else if (!isspace(str[i])) //another text char found  (save only non-space chars)
                {
                    *end = &str[i];
                }
            }
        }
    }
}