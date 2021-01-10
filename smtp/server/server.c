#include "../utils/util.h"

int main() {

	int socket_listen = setup_TCP_server(PORT);

	/* poll() is used to wait for an event on one or more sockets */

	struct pollfd pollfds[MAX_CLIENTS + 1];

	pollfds[0].fd = socket_listen;
	pollfds[0].events = POLLIN | POLLPRI;

	for (int i = 1; i < MAX_CLIENTS; i++) {
		pollfds[i].fd = -1; // -1 indicates available entry
	}

	int useClient = 0;

	/* Create array for store state of server for each client */

	// int state_server[MAX_CLIENTS];

	// for (int i = 1; i < MAX_CLIENTS; i++) { // socket client start from 1 too (see in poll())
	// 	state_server[i] = INIT;
	// }

	/* Communication */

	printf("Waiting for connections...\n");

	while(1) {

		if (poll(pollfds, useClient + 1, TIMEINF) < 0) {
			fprintf(stderr, "poll() failed. (%d)\n", errno);
			exit(1);
		}

		if (pollfds[0].revents & POLLIN) {
			struct sockaddr_storage client_address;
			socklen_t client_len = sizeof(client_address);

			/* accept() is used on the server to create a new socket for an incoming TCP connection */

			int socket_client = accept(socket_listen, (struct sockaddr *) &client_address, &client_len);

			if (socket_client < 0) {
				fprintf(stderr, "accept() failed. (%d)\n", errno);
				exit(1);
			}

			// Configure new socket
			for (int i = 1; i < MAX_CLIENTS; i++) {
				if (pollfds[i].fd < 0) {
					pollfds[i].fd = socket_client;
					pollfds[i].events = POLLIN | POLLPRI;

					// state_server[i] = BEGIN;

					useClient++;
					break;
				}
			}

			// Print information of client
			char address_buffer[100];

			getnameinfo((struct sockaddr*) &client_address, client_len, 
				address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);

			printf("New connection from %s\n", address_buffer);

			// Send 220 code to client
			char msg[BUFSIZE];

			strcpy(msg, "220 Simple Mail Transfer Service Ready\n"); // "\n" = Enter
			send(socket_client, msg, strlen(msg), 0);
		}

		/* send() and recv() are used to send and receive data with a socket */

		for (int i = 1; i < MAX_CLIENTS; i++) {

			if (pollfds[i].fd < 0) {
				continue;
			}

			if (pollfds[i].revents & POLLIN) {
				char read[BUFSIZE];

				/* Receive new data */

				int bytes_received = recv(pollfds[i].fd, read, BUFSIZE, 0);

				if (bytes_received < 1) {
					pollfds[i].fd = -1;
					pollfds[i].events = -1;
					pollfds[i].revents = -1;

					// state_server[i] = INIT;

					useClient--;
					close(pollfds[i].fd);
				} else {

					/* Send new data */

					send(pollfds[i].fd, read, bytes_received, 0);
				}
			}
		}

	}

	close_server_socket(socket_listen);

    return 0;
}
