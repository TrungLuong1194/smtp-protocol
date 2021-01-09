#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#define MAX_CLIENTS 10

int main() {

	/* Configure local */

	printf("Configuring local address...\n");

	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo *bind_address; // local address

	getaddrinfo(0, "8080", &hints, &bind_address);

	/* socket() creates and initializes a new socket */

	printf("Creating socket...\n");

	int socket_listen;

	socket_listen = socket(bind_address->ai_family,
		bind_address->ai_socktype, bind_address->ai_protocol);

	if (socket_listen < 0) {
		fprintf(stderr, "socket() failed. (%d)\n", errno);
		return 1;
	}

	/* bind() associates a socket with a particular local IP address and port number */

	printf("Binding socket to local address...\n");

	if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen) < 0) {
		fprintf(stderr, "bind() failed. (%d)\n", errno);
		return 1;
	}

	freeaddrinfo(bind_address);

	/* listen() is used on the server to cause a TCP socket to listen for new connections */

	printf("Listening...\n");

	if (listen(socket_listen, 10) < 0) {
		fprintf(stderr, "listen() failed. (%d)\n", errno);
		return 1;
	}

	/* poll() is used to wait for an event on one or more sockets */

	struct pollfd pollfds[MAX_CLIENTS + 1];

	pollfds[0].fd = socket_listen;
	pollfds[0].events = POLLIN | POLLPRI;

	for (int i = 1; i < MAX_CLIENTS; i++) {
		pollfds[i].fd = -1; // -1 indicates available entry
	}

	int useClient = 0;

	/* Communication */

	printf("Waiting for connections...\n");

	while(1) {

		if (poll(pollfds, useClient + 1, -1) < 0) {
			fprintf(stderr, "poll() failed. (%d)\n", errno);
			return 1;
		}

		if (pollfds[0].revents & POLLIN) {
			struct sockaddr_storage client_address;
			socklen_t client_len = sizeof(client_address);

			/* accept() is used on the server to create a new socket for an incoming TCP connection */

			int socket_client = accept(socket_listen, (struct sockaddr *) &client_address, &client_len);

			if (socket_client < 0) {
				fprintf(stderr, "accept() failed. (%d)\n", errno);
				return 1;
			}

			// Configure new socket
			for (int i = 1; i < MAX_CLIENTS; i++) {
				if (pollfds[i].fd < 0) {
					pollfds[i].fd = socket_client;
					pollfds[i].events = POLLIN | POLLPRI;

					useClient++;
					break;
				}
			}

			// Print information of client
			char address_buffer[100];

			getnameinfo((struct sockaddr*) &client_address, client_len, 
				address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);

			printf("New connection from %s\n", address_buffer);
		}

		/* send() and recv() are used to send and receive data with a socket */

		for (int i = 1; i < MAX_CLIENTS; i++) {

			if (pollfds[i].fd < 0) {
				continue;
			}

			if (pollfds[i].revents & POLLIN) {
				char read[1024];

				/* Receive new data */

				int bytes_received = recv(pollfds[i].fd, read, 1024, 0);

				if (bytes_received < 1) {
					pollfds[i].fd = -1;
					pollfds[i].events = -1;
					pollfds[i].revents = -1;

					useClient--;
					close(pollfds[i].fd);
				} else {

					/* Send new data */

					send(pollfds[i].fd, read, bytes_received, 0);
				}
			}
		}

	}

	printf("Closing listening socket...\n");
    close(socket_listen);

    printf("Finished.\n");
    return 0;
}