#ifndef UTILLIB_H
#define UTILLIB_H

#include <stdio.h>
#include <sys/socket.h>

enum dataConstants {
	BUFSIZE = 512,
	MAXPENDING = 5, // Maximum outstanding connection requests
};

// Handle error with user
void errorUser(char *msg, char *detail);

// Handle error with system
void errorSystem(char *msg);

// Handle new TCP client
void HandleTCPClient(int clntSocket);

#endif
