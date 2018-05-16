#ifndef PARSER_H
#define PARSER_H

#define msg_size 80 //size in bytes

struct msg {
    long mtype; // > 0
    char mtext[msg_size]; //payload
};

#endif
