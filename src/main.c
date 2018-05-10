#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include "./lib/commands.h"
#include "./parser/parser.h"
#include "./executer/executer.h"

#define MAXLEN 80

int main(int argc, char *argv[])
{
    struct Command *result = parseCommand(argc, argv);

    char *p = result->command;
    char *start = NULL;
    char *end = NULL;
    int subIndex = 0;

    do
    {
        getNextSubCommand(p, &start, &end);
        p = end + 1;
        if (start != NULL && end != NULL)
        {
            struct SubCommandResult *subcommand = malloc(sizeof(struct SubCommandResult));
            result->subCommands[subIndex] = subcommand;
            subIndex += 1;

            int length = (end - start) * sizeof(*start) + 1;
            sprintf(subcommand->subCommand, "%.*s", length, start);
            printf("%.*s\n", length, start);
            pid_t fid = executeSubCommand(subcommand);
            printf("PID of process that executed command: %d\n", fid);
        }

    } while (start != NULL && end != NULL);

    // Freeing dynamically allocated memory
    for (int i = 0; i < subIndex; i++)
    {
        free(result->subCommands[i]);
    }
    free(result);

    return 0;
}