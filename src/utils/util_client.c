#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <stdarg.h>
#include "util.h"

/* Get the email server hostname, the recipient address ... from input stdin */

void get_input(const char *prompt, char *buffer) {

	printf("%s", prompt);

	buffer[0] = 0; // 0 = null in ascii table
	fgets(buffer, MAXINPUT, stdin);

	const int read = strlen(buffer);
	if (read > 0) {
		buffer[read - 1] = 0;
	}
}

/* 
	Send formatted strings directly over the network.
	http://www.cplusplus.com/reference/cstdio/vsprintf/
*/

void send_format(int server, const char *text, ...) {

	char buffer[BUFSIZE];

	va_list args;
	va_start(args, text);
	vsprintf(buffer, text, args);
	va_end(args);

	send(server, buffer, strlen(buffer), 0);

	printf("C: %s\n", buffer);
}

/* 
	Parsing the SMTP server responses.
	Return response code.
	Ex: repsonse = "250 Message received!" -> return: 250
*/

int parse_response(const char *response) {

    const char *k = response;

    if (!k[0] || !k[1] || !k[2]) { // check null terminate
    	return 0;
    }

    for (; k[3]; ++k) {
        if (k == response || k[-1] == '\n') {
            if (isdigit(k[0]) && isdigit(k[1]) && isdigit(k[2])) {
                if (k[3] != '-') {
                    if (strstr(k, "\r\n")) { // "\r\n" = End Of Line
                        return strtol(k, 0, 10);
                    }
                }
            }
        }
    }

    return 0;
}

/* Waits until a particular response code is received over the network */

void wait_on_response(int server, int expecting) {

    char response[MAXRESPONSE + 1];
    char *p = response;
    char *end = response + MAXRESPONSE;

    int code = 0;

    do {
        int bytes_received = recv(server, p, end - p, 0);

        if (bytes_received < 1) {
            fprintf(stderr, "Connection dropped.\n");
            exit(1);
        }

        p += bytes_received;
        *p = 0;

        if (p == end) {
            fprintf(stderr, "Server response too large:\n");
            fprintf(stderr, "%s", response);
            exit(1);
        }

        code = parse_response(response);

    } while (code == 0);

    if (code != expecting) {
        fprintf(stderr, "Error from server:\n");
        fprintf(stderr, "%s", response);
        exit(1);
    }

    printf("S: %s", response);
}

/* Open a TCP connection to a given server and port number */

int connect_to_server(const char *server, const char *port) {

	/* Configure remote */

	printf("Configuring remote address...\n");

	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM; // TCP connection

	struct addrinfo *peer_address; // remote address

	if (getaddrinfo(server, port, &hints, &peer_address)) {
		fprintf(stderr, "getaddrinfo() failed. (%d)\n", errno);
		return 1;
	}

	/* Print information of remote */

	printf("Remote address is: ");

	char address_buffer[ADDRSIZE];
	char service_buffer[ADDRSIZE];

	getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen, 
		address_buffer, sizeof(address_buffer), 
		service_buffer, sizeof(service_buffer),
		NI_NUMERICHOST);

	printf("%s %s\n", address_buffer, service_buffer);

	/* socket() creates and initializes a new socket */

	printf("Creating socket...\n");

	int socket_peer;

	socket_peer = socket(peer_address->ai_family,
		peer_address->ai_socktype, peer_address->ai_protocol);

	if (socket_peer < 0) {
		fprintf(stderr, "socket() failed. (%d)\n", errno);
		return 1;
	}

	/* connect() is used on the client to set the remote address and port */

	printf("Connecting...\n");

	if (connect(socket_peer, peer_address->ai_addr, peer_address->ai_addrlen) < 0) {
		fprintf(stderr, "connect() failed. (%d)\n", errno);
		return 1;
	}

	freeaddrinfo(peer_address);

	/* send() and recv() are used to send and receive data with a socket */

	printf("Connected.\n");

    return socket_peer;
}
