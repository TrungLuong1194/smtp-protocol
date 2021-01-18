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
#include <time.h>
#include <stdarg.h>

/* Constants */

#define MASTER 					0
#define MAXINPUT 				1024
#define BUFSIZE 				4096
#define LOGSIZE					100
#define SIZE 					100
#define TIME_INF 				-1
#define INIT_VALUE 				-1
#define MAX_CLIENTS 			10
#define PORT 					"2525"
#define TRUE 					1
#define FALSE 					0
#define INFO					"INFO"
#define ERROR					"ERROR"
#define NUM_RECORD				10

/* Server State */

#define INIT_STATE 					0
#define BEGIN_STATE 				1
#define WAIT_STATE 					2
#define ENVELOPE_CREATED_STATE 		3
#define RECIPIENTS_SET_STATE 		4
#define WRITING_MAIL_STATE 			5
#define READY_TO_DELIVER_STATE 		6
#define QUIT_STATE 					7

/* Regex message from client */

#define HELO_CMD "(h|H)(e|E)(l|L)(o|O)\\s+.+"
#define EHLO_CMD "(e|E)(h|H)(l|L)(o|O)\\s+.+"
#define MAIL_CMD "(m|M)(a|A)(i|I)(l|L)\\s+(f|F)(r|R)(o|O)(m|M)\\s*:\\s*<(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+>"
#define RCPT_CMD "(r|R)(c|C)(p|P)(t|T)\\s+(t|T)(o|O)\\s*:\\s*<(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+>"
#define DATA_CMD "(d|D)(a|A)(t|T)(a|A)"
#define RSET_CMD "(r|R)(s|S)(e|E)(t|T)"
#define VRFY_CMD "(v|V)(r|R)(f|F)(y|Y)\\s+.+"
#define QUIT_CMD "(q|Q)(u|U)(i|I)(t|T)"

/* Client */

int setup_TCP_client(const char *server, const char *port);
void get_input(const char *prompt, char *buffer);
void close_client_socket(const int socket);

/* Server */

int setup_TCP_server(const char *port);
void close_server_socket(const int socket);

/* For both client and server */

int is_matching_pattern(const char *str, const char *pattern);
void logs(const char *filename, const char *level, const char *text, ...);
char *get_address_user(const char *name);
char *get_hostname(char* input);

#endif
