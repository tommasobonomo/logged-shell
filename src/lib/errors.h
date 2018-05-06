#ifndef ERRORS_H
#define ERRORS_H

#define ERR_X 0
#define ERR_BAD_ARGS 1
#define ERR_BAD_ARG_X 2
#define ERR_UNKNOWN_ARG_X 3

/**
 * Useful function to print a template error and exit the program<br>
 * It performs an <code>exit(id)</code> at the end
 * @param id The message id, useful for the template. Exit code<br>
 *           Can be one of the ERR_...
 * @param msg Custom message that will be insert in the template
 */
void error_fatal(int id, char *msg);

#endif