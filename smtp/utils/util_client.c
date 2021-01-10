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

#include "util.h"

/* Open a TCP connection to a given server and port number */

int setupTCPClient(const char *server, const char *port) {

	/* Configure remote */

	printf("Configuring remote address...\n");

	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM; // TCP connection

	struct addrinfo *peer_address; // remote address

	if (getaddrinfo(server, port, &hints, &peer_address)) {
		fprintf(stderr, "getaddrinfo() failed. (%d)\n", errno);
		exit(1);
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
		exit(1);
	}

	/* connect() is used on the client to set the remote address and port */

	printf("Connecting...\n");

	if (connect(socket_peer, peer_address->ai_addr, peer_address->ai_addrlen) < 0) {
		fprintf(stderr, "connect() failed. (%d)\n", errno);
		exit(1);
	}

	freeaddrinfo(peer_address);

	/* send() and recv() are used to send and receive data with a socket */

	printf("Connected.\n");

    return socket_peer;
}

/* Get data from input stdin */

void get_input(const char *prompt, char *buffer) {

	printf("%s", prompt);

	buffer[0] = 0; // 0 = null in ascii table
	fgets(buffer, MAXINPUT, stdin);

	const int read = strlen(buffer);
	if (read > 0) {
		buffer[read - 1] = 0;
	}
}

/* Communication with server using poll() */

void communication_with_server(const int socket_peer) {

	while(1) {

		struct pollfd pollfds[2];

		// file description for stdin
		pollfds[0].fd = 0;
		pollfds[0].events = POLLIN | POLLPRI;

		// file description for socket
		pollfds[1].fd = socket_peer;
		pollfds[1].events = POLLIN | POLLPRI;

		/* poll() is used to wait for an event on one or more sockets */

		if (poll(pollfds, 2, TIMEOUT) < 0) {
			fprintf(stderr, "poll() failed. (%d)\n", errno);
			exit(1);
		}

		/* Receive new data */

		if (pollfds[1].revents & POLLIN) { // check if a file descriptor in socket set
			char read[BUFSIZE];

			int bytes_received = recv(socket_peer, read, BUFSIZE, 0);

			if (bytes_received < 1) {
				printf("Connection closed by peer.\n");
				break;
			}

			printf("<< %s", read);
		}

		/* Send new data */

		if (pollfds[0].revents & POLLIN) { // check stdin file descriptor: 0
			char read[BUFSIZE];

			// fgets() includes the newline character from the input.
			if (!fgets(read, BUFSIZE, stdin)) { // Check ends with a newline
				break;
			}

			// int bytes_sent = send(socket_peer, read, strlen(read), 0);
			send(socket_peer, read, strlen(read), 0);
		}
	}
}

/* Close a socket */
void close_socket(const int socket) {

	printf("Closing socket...\n");

	close(socket);

	printf("Finished.\n");
}
