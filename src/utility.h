#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>
#include <sys/socket.h>

enum dataConstants {
	BUFSIZE = 512,
};

// Handle error with user
void errorUser(char *msg, char *detail);

// Handle error with system
void errorSystem(char *msg);

#endif
