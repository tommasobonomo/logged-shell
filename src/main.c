#include <stdio.h>
#include <stdlib.h>
#include "./parser/parser.h"


int main(int argc, char *argv[])
{
    struct Command *result = parseCommand(argc, argv);
    free(result);

    return 0;
}