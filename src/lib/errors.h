#ifndef ERRORS_H
#define ERRORS_H

#define ERR_X 1
#define ERR_BAD_ARGS 2
#define ERR_BAD_ARG_X 3
#define ERR_UNKNOWN_ARG_X 4
#define ERR_IO_FILE 5
#define ERR_SYSCALL 6
#define ERR_EXEC 7
#define ERR_SIGNAL 8

/**
 * Useful function to print a template error and exit the program<br>
 * It performs an <code>exit(id)</code> at the end
 * @param id The message id, useful for the template. Exit code<br>
 *           Can be one of the ERR_...
 * @param msg Custom message that will be insert in the template
 * @param exit_status Exit status to use for exit
 */
void error_fatal(int id, char const *msg, int exit_status);

/**
 * Instead of closing the program in case of errors, it just warns the user
 * @param id The message id, useful for the template
 * @param msg Custom message that will be insert in the template
 */
void error_warning(int id, char const *msg);

#endif
