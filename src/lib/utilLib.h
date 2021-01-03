#ifndef UTILLIB_H
#define UTILLIB_H

#include <stdio.h>
#include <sys/socket.h>

enum dataConstants {
	BUFSIZE = 512,
	MAXPENDING = 5, // Maximum outstanding connection requests
};

/* ---- Error ---- */

// Handle error with user
void errorUser(const char *msg, const char *detail);

// Handle error with system
void errorSystem(const char *msg);

/* ---- Client ---- */

// Handle new TCP client
void handleTCPClient(int clntSocket);

// Create and connect a new TCP client socket
int setupTCPClientSocket(const char *host, const char *service);

/* ---- Server ---- */

// Print socket address
void printSocketAddress(const struct sockaddr *address, FILE *stream);

// Create, bind, and listen a new TCP server socket
int setupTCPServerSocket(const char *service);

// Accept a new TCP connection on a server socket
int acceptTCPConnection(int servSock);

#endif
