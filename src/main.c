#include <stdio.h>
#include <stdlib.h>
#include "./lib/commands.h"
#include "./parser/parser.h"


int main(int argc, char *argv[])
{
    struct Command *result = parseCommand(argc, argv);

    char *p = result->command;
    char *start = NULL;
    char *end = NULL;
    do
    {
        getNextSubCommand(p, &start, &end);
        p = end + 1;
        if (start != NULL && end != NULL)
        {
            int length = (end - start) * sizeof(*start) + 1;
            printf("\"%.*s\"\n", length, start);
        }

    } while (start != NULL && end != NULL);

    free(result);

    return 0;
}