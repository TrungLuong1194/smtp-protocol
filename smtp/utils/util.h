#ifndef UTIL_H
#define UTIL_H

enum constants {
	MAXINPUT = 512,
	MAXRESPONSE = 1024,
	BUFSIZE = 1024,
	ADDRSIZE = 100,
};

/* ---- Client ---- */

void get_input(const char *prompt, char *buffer);
void send_format(int server, const char *text, ...);
int parse_response(const char *response);
void wait_on_response(int server, int expecting);
int connect_to_server(const char *server, const char *port);

/* ---- Server ---- */

#endif
