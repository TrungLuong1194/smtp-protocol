#ifndef UTIL_H
#define UTIL_H

#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>

/* Constants */

#define MAXINPUT 1024
#define MAXRESPONSE 1024
#define BUFSIZE 4096
#define ADDRSIZE 100
#define TIMEOUT 100000
#define TIMEINF -1
#define MAX_CLIENTS 10
#define PORT "2525"

/* Server State */

#define INIT -1
#define BEGIN 1
#define WAIT 2
#define ENVELOPE_CREATED 3
#define RECIPIENTS_SET 4
#define WRITING_MAIL 5
#define READY_TO_DELIVER 6
#define QUIT 7

/* Client */

int setup_TCP_client(const char *server, const char *port);
void get_input(const char *prompt, char *buffer);
void close_client_socket(const int socket);

/* Server */

int setup_TCP_server(const char *port);
void close_server_socket(const int socket);

#endif
