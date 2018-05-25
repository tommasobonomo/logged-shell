#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../lib/errors.h"
#include "../daemon/daemon.h"
#include "parser.h"

/**
 * Add all the default parameters
 * @param cmd the struct in which add the parameters
 */
void addDefault(Command *cmd)
{
    cmd->command[0] = '\0';
    strcpy(cmd->log_path, DEFAULT_LOGPATH_TXT);
    cmd->log_format = LOG_FORMAT_TXT;
    cmd->output_mode = MODE_SCREEN;
    cmd->output_path[0] = '\0';
    cmd->error_mode = MODE_SCREEN;
    cmd->error_path[0] = '\0';
    cmd->n_subCommands = 0;
}

void setCommand(Command *cmd, char *str_cmd)
{
    if (cmd->command[0] != '\0')
    {
        error_fatal(ERR_X, "command alredy specified");
    }
    else
    {
        strcpy(cmd->command, str_cmd);
    }
}

void setLogfile(Command *cmd, char *path)
{
    if (strcmp(cmd->log_path, DEFAULT_LOGPATH_TXT) != 0 && strcmp(cmd->log_path, DEFAULT_LOGPATH_CSV) != 0)
    {
        error_fatal(ERR_X, "log path alredy specified");
    }
    else
    {
        strcpy(cmd->log_path, path);
    }
}

void setLogformat(Command *cmd, char *format)
{
    if (strcmp(format, "txt") == 0)
    {
        cmd->log_format = LOG_FORMAT_TXT;
    }
    else if (strcmp(format, "csv") == 0)
    {
        cmd->log_format = LOG_FORMAT_CSV;
        setLogfile(cmd, DEFAULT_LOGPATH_CSV);
    }
    else
    {
        error_fatal(ERR_BAD_ARG_X, format);
    }
}

void setOutputMode(Command *cmd, int mode)
{
    cmd->output_mode = mode;
}

void setErrorMode(Command *cmd, int mode)
{
    cmd->error_mode = mode;
}

void setOutputPath(Command *cmd, char *path)
{
    if (cmd->output_path[0] != '\0')
    {
        error_fatal(ERR_X, "output path alredy specified");
    }
    else
    {
        strcpy(cmd->output_path, path);
    }
}

void setErrorPath(Command *cmd, char *path)
{
    if (cmd->error_path[0] != '\0')
    {
        error_fatal(ERR_X, "error path alredy specified");
    }
    else
    {
        strcpy(cmd->error_path, path);
    }
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

Command *parseCommand(int argc, char *argv[])
{
    Command *result = malloc(sizeof(Command));
    addDefault(result);
    DEBUG_PRINT("Command allocato, default added\n\n");

    int currArgc;
    for (currArgc = 1; currArgc < argc; currArgc++)
    {
        if (argv[currArgc][0] == '-')
        {
            if (argv[currArgc][1] != '-')
            {
                //NORMAL ARG
                if (strStartWith(&argv[currArgc][1], ARG_LOGPATH))
                {
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setLogfile(result, secondaryArg);
                    DEBUG_PRINT("path logfile: %s\n", result->log_path);
                }
                else if (strStartWith(&argv[currArgc][1], ARG_LOGFORMAT))
                {
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setLogformat(result, secondaryArg); // Replaces default logpath if needed
                    DEBUG_PRINT("format: %d\n", result->log_format);
                }
                else if (strStartWith(&argv[currArgc][1], ARG_OUTSCREEN))
                {
                    setOutputMode(result, MODE_SCREEN);
                    DEBUG_PRINT("outputmode: %d\n", result->output_mode);
                }
                else if (strStartWith(&argv[currArgc][1], ARG_ERRSCREEN))
                {
                    setErrorMode(result, MODE_SCREEN);
                    DEBUG_PRINT("errormode: %d\n", result->error_mode);
                }
                else if (strStartWith(&argv[currArgc][1], ARG_OUTFILEAPP))
                {
                    setOutputMode(result, MODE_FILEAPP);
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setOutputPath(result, secondaryArg);
                    DEBUG_PRINT("outputmode: %d\n", result->output_mode);
                    DEBUG_PRINT("outputpath: %s\n", result->output_path);
                }
                else if (strStartWith(&argv[currArgc][1], ARG_ERRFILEAPP))
                {
                    setErrorMode(result, MODE_FILEAPP);
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setErrorPath(result, secondaryArg);
                    DEBUG_PRINT("errormode: %d\n", result->error_mode);
                    DEBUG_PRINT("errorpath: %s\n", result->error_path);
                }
                else if (strStartWith(&argv[currArgc][1], ARG_OUTFILEOVER))
                {
                    setOutputMode(result, MODE_FILEOVER);
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setOutputPath(result, secondaryArg);
                    DEBUG_PRINT("outputmode: %d\n", result->output_mode);
                    DEBUG_PRINT("outputpath: %s\n", result->output_path);
                }
                else if (strStartWith(&argv[currArgc][1], ARG_ERRFILEOVER))
                {
                    setErrorMode(result, MODE_FILEOVER);
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setErrorPath(result, secondaryArg);
                    DEBUG_PRINT("errormode: %d\n", result->error_mode);
                    DEBUG_PRINT("errorpath: %s\n", result->error_path);
                }
                else
                {
                    error_fatal(ERR_UNKNOWN_ARG_X, argv[currArgc]);
                }
            }
            else
            {
                //MNEMONIC ARG:
                if (strStartWith(&argv[currArgc][2], ARG_MNEM_HELP))
                {
                    printHelpAndExit();
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MNEM_LOGPATH))
                {
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setLogfile(result, secondaryArg);
                    DEBUG_PRINT("path logfile: %s\n", result->log_path);
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MNEM_LOGFORMAT))
                {
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setLogformat(result, secondaryArg); // Replaces default logpath if needed
                    DEBUG_PRINT("format: %d\n", result->log_format);
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MNEM_OUTDISCARD))
                {
                    setOutputMode(result, MODE_DISCARD);
                    DEBUG_PRINT("output_mode: %d\n", result->output_mode);
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MNEM_ERRDISCARD))
                {
                    setErrorMode(result, MODE_DISCARD);
                    DEBUG_PRINT("error_mode: %d\n", result->error_mode);
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MNEM_OUTSCREEN))
                {
                    setOutputMode(result, MODE_SCREEN);
                    DEBUG_PRINT("output_mode: %d\n", result->output_mode);
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MNEM_ERRSCREEN))
                {
                    setErrorMode(result, MODE_SCREEN);
                    DEBUG_PRINT("error_mode: %d\n", result->error_mode);
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MNEM_OUTFILEAPP))
                {
                    setOutputMode(result, MODE_FILEAPP);
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setOutputPath(result, secondaryArg);
                    DEBUG_PRINT("output_mode: %d\n", result->output_mode);
                    DEBUG_PRINT("output_path: %s\n", result->output_path);
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MNEM_ERRFILEAPP))
                {
                    setErrorMode(result, MODE_FILEAPP);
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setErrorPath(result, secondaryArg);
                    DEBUG_PRINT("error_mode: %d\n", result->error_mode);
                    DEBUG_PRINT("error_path: %s\n", result->error_path);
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MNEM_OUTFILEOVER))
                {
                    setOutputMode(result, MODE_FILEOVER);
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setOutputPath(result, secondaryArg);
                    DEBUG_PRINT("output_mode: %d\n", result->output_mode);
                    DEBUG_PRINT("output_path: %s\n", result->output_path);
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MNEM_ERRFILEOVER))
                {
                    setErrorMode(result, MODE_FILEOVER);
                    char *secondaryArg = getSecondaryArg(argc, argv, &currArgc);
                    setErrorPath(result, secondaryArg);
                    DEBUG_PRINT("error_mode: %d\n", result->error_mode);
                    DEBUG_PRINT("error_path: %s\n", result->error_path);
                }
                else if (strStartWith(&argv[currArgc][2], ARG_MNEM_VERSION))
                {
                    printVersionAndExit();
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

    int i;
    for (i = 0; !exit; i++)
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
