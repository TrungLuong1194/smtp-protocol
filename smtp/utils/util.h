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
#include <regex.h>

/* Constants */

#define MAXINPUT 1024
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
#define EXIT 7

/* Regex message from client */

#define HELO "(h|H)(e|E)(l|L)(o|O)\\s+.+"
#define EHLO "(e|E)(h|H)(l|L)(o|O)\\s+.+"
#define MAIL "(m|M)(a|A)(i|I)(l|L)\\s+(f|F)(r|R)(o|O)(m|M)\\s*:\\s*<(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+>"
#define RCPT "(r|R)(c|C)(p|P)(t|T)\\s+(t|T)(o|O)\\s*:\\s*<(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+>"
#define DATA "(d|D)(a|A)(t|T)(a|A)"
#define RSET "(r|R)(s|S)(e|E)(t|T)"
#define QUIT "(q|Q)(u|U)(i|I)(t|T)"

/* Client */

int setup_TCP_client(const char *server, const char *port);
void get_input(const char *prompt, char *buffer);
void close_client_socket(const int socket);

/* Server */

int setup_TCP_server(const char *port);
void close_server_socket(const int socket);
int is_matching_pattern(const char *str, const char *pattern);

#endif
