#include <stdio.h>
#include <stdlib.h>

/**
 * Executes subcommand received from main file
 * @param subcommand the subcommand that will be executed
 */
void executeSubCommand(char subcommand[])
{
    system(subcommand);
}