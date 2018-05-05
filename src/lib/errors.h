#ifndef ERRORS_H
#define ERRORS_H

#define ERR_X 0
#define ERR_BAD_ARGS 1
#define ERR_BAD_ARG_X 2
#define ERR_UNKNOWN_ARG_X 3

void error_fatal(int id, char *msg);

#endif
