#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>
#include <sys/socket.h>

// Handle error with user
void errorUser(char *msg, char *detail);

// Handle error with system
void errorSystem(char *msg);

enum dataConstants {
	BUFSIZE = 512,
};

#endif
