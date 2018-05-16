#ifndef MESSAGE_H
#define MESSAGE_H

#define maxstr 80

typedef struct msg {
	long type; // > 0
	char text[maxstr]; //payload
} msg;

#define msgsz maxstr*sizeof(char)

#define PERMS 0600

#endif
