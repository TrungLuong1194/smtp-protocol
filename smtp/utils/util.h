#ifndef UTIL_H
#define UTIL_H

/* Constants */

#define MAXINPUT 1024
#define MAXRESPONSE 1024
#define BUFSIZE 4096
#define ADDRSIZE 100
#define TIMEOUT 100000

/* Server State */

#define BEGIN 1
#define WAIT 2
#define ENVELOPE_CREATED 3
#define RECIPIENTS_SET 4
#define WRITING_MAIL 5
#define READY_TO_DELIVER 6
#define QUIT 7

/* Client */

int setupTCPClient(const char *server, const char *port);
void get_input(const char *prompt, char *buffer);
void communication_with_server(const int socket_peer);
void close_socket(const int socket);

/* Server */

#endif
