#ifndef MESSAGE_H
#define MESSAGE_H

#define maxstr 80

typedef struct message {
	long type; // > 0
	char text[maxstr]; //payload
} message;

#define msgsz maxstr*sizeof(char)

#define PERMS 0600

#endif
