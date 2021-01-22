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
#include <sys/stat.h>
#include <uuid/uuid.h>

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
#define NUM_RECORD				5
#define SIGQUIT					"221 BYE\n"
#define MAX_CC					10
#define SERVER_LOG_DIR			"../../logs/server.log"
#define CLIENT_LOG_DIR			"../../logs/client.log"

/* Regex message from client */

#define HELO_CMD "(h|H)(e|E)(l|L)(o|O)\\s+.+"
#define EHLO_CMD "(e|E)(h|H)(l|L)(o|O)\\s+.+"
#define MAIL_CMD "(m|M)(a|A)(i|I)(l|L)\\s+(f|F)(r|R)(o|O)(m|M)\\s*:\\s*<(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+>"
#define RCPT_CMD "(r|R)(c|C)(p|P)(t|T)\\s+(t|T)(o|O)\\s*:\\s*<(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+>"
#define DATA_CMD "(d|D)(a|A)(t|T)(a|A)\n"
#define RSET_CMD "(r|R)(s|S)(e|E)(t|T)\n"
#define VRFY_CMD "(v|V)(r|R)(f|F)(y|Y)\\s+.+"
#define QUIT_CMD "(q|Q)(u|U)(i|I)(t|T)\n"

/* Server States */

typedef enum {

	Init_State,
	Begin_State,
	Wait_State,
	Envelope_Create_State,
	Recipients_Set_State,
	Writing_Mail_State,
	Ready_To_Deliver_State,

} eSystemState;

/* Server Events */

typedef enum {

	Helo_Event,
	Ehlo_Event,
	Mail_Event,
	Rcpt_Event,
	Data_Event,
	Rset_Event,
	Vrfy_Event,
	Quit_Event,
	Undefined_Event,

} eSystemEvent;

/* Mailbox record */

struct mailbox {
	int id;
	char *hostname;
	char *address;
	char *dirname1;
	char *dirname2;
};

/* Mail Content */

struct mail {
	char from[SIZE];
	char to[SIZE];
	char cc[MAX_CC][SIZE];
	struct tm datetime;
	char body[BUFSIZE];
};

/* Client */

int setup_TCP_client(const char *server, const char *port);
void get_input(const char *prompt, char *buffer);
void close_client_socket(const int socket);

/* Server */

int setup_TCP_server(const char *port);
void close_server_socket(const int socket);
void send_mail(struct mail mc, int num_cc);

/* For both client and server */

int is_matching_pattern(const char *str, const char *pattern);
struct tm get_time();
void logs(const char *filename, const char *level, const char *text, ...);
struct mailbox *init_mailbox();
char *get_address_mail_from_hostname(const char *name);
char *get_hostname_from_address_mail(const char *addr);
char *get_hostname_from_vrfy(char* input);
void get_address_from_rcpt_or_mail(char *input, char *output);
int check_address_in_mailbox(char *mail);

/* FSM */

eSystemEvent event_trigger(char *response);
eSystemState fsm_state_handler(struct pollfd *pfds, int *nfds, char *response, eSystemState eCurrentState, 
	eSystemEvent eNewEvent, struct mail *mc, int *num_cc);

#endif
