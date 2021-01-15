#include "../utils/util.h"

int main() {

	int socket_listen = setup_TCP_server(PORT);

	// poll() is used to wait for an event on one or more sockets
	struct pollfd pollfds[MAX_CLIENTS + 1];

	pollfds[MASTER].fd = socket_listen;
	pollfds[MASTER].events = POLLIN | POLLPRI;

	for (int i = 1; i <= MAX_CLIENTS; i++) {
		pollfds[i].fd = AVAILABLE_ENTRY; // -1 indicates available entry
	}

	int useClient = 0;

	// Create array for store state of server for each client
	int current_state[MAX_CLIENTS];

	for (int i = 1; i <= MAX_CLIENTS; i++) { // socket client start from 1 too (see in poll())
		current_state[i] = INIT_STATE;
	}

	printf("Waiting for connections...\n");

	while(TRUE) {

		if (poll(pollfds, useClient + 1, TIME_INF) < 0) {
			fprintf(stderr, "poll() failed. (%d)\n", errno);
			exit(1);
		}

		// Action from server socket
		if (pollfds[MASTER].revents & POLLIN) {

			struct sockaddr_storage client_address;
			socklen_t client_len = sizeof(client_address);

			// accept() is used on the server to create a new socket for an incoming TCP connection
			int socket_client = accept(socket_listen, (struct sockaddr *) &client_address, &client_len);

			if (socket_client < 0) {
				fprintf(stderr, "accept() failed. (%d)\n", errno);
				exit(1);
			}

			// Configure new socket
			for (int i = 1; i <= MAX_CLIENTS; i++) {
				if (pollfds[i].fd < 0) {
					pollfds[i].fd = socket_client;
					pollfds[i].events = POLLIN | POLLPRI;

					current_state[i] = BEGIN_STATE;

					useClient++;
					break;
				}
			}

			// Print information of client
			char address_buffer[SIZE];

			getnameinfo((struct sockaddr*) &client_address, client_len, 
				address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);

			printf("New connection from %s\n", address_buffer);

			// Send 220 code
			char msg[] = "220 localhost Simple Mail Transfer Service Ready\n";

			send(socket_client, msg, strlen(msg), 0);
		}

		// Action from client socket
		for (int i = 1; i <= MAX_CLIENTS; i++) {

			if (pollfds[i].fd < 0) {
				continue;
			}

			if (pollfds[i].revents & POLLIN) {

				char response[BUFSIZE + 1];
				char *p = response;
				char *end = response + BUFSIZE;

				int bytes_received = recv(pollfds[i].fd, p, end - p, 0);

				if (bytes_received < 1) {
					pollfds[i].fd = AVAILABLE_ENTRY;
					pollfds[i].events = AVAILABLE_ENTRY;
					pollfds[i].revents = AVAILABLE_ENTRY;

					current_state[i] = INIT_STATE;

					useClient--;
					close(pollfds[i].fd);

					continue;
				}

				p += bytes_received;
				*p = 0;

				if (p == end) {
		            fprintf(stderr, "Client message too large:\n");
		            fprintf(stderr, "%s", response);
		            exit(1);
		        }

				// Send data back to client
				char str[bytes_received];

				for (int j = 0; j < bytes_received; j++) {
					str[j] = response[j];
				}

				/* ---------- STATE BEGIN ---------- */

				if (current_state[i] == BEGIN_STATE) {
					if (is_matching_pattern(str, HELO_CMD) == TRUE) {
						char msg[] = "250 localhost is on the air\n";

						send(pollfds[i].fd, msg, strlen(msg), 0);

						current_state[i] = WAIT_STATE; }
					// } else if (is_matching_pattern(str, EHLO) == 1) {
					// 	char msg[] = "250-localhost\n250-8BITMIME\n250-SIZE\n250 OK\n";

					// 	send(pollfds[i].fd, msg, strlen(msg), 0);

					// 	current_state[i] = WAIT;
					// } else if (is_matching_pattern(str, MAIL) == 1) {
					// 	char msg[] = "503 Error: send HELO/EHLO first\n";

					// 	send(pollfds[i].fd, msg, strlen(msg), 0);
					// } else if (is_matching_pattern(str, RCPT) == 1) {
					// 	char msg[] = "503 Error: need MAIL command\n";

					// 	send(pollfds[i].fd, msg, strlen(msg), 0);
					// } else if (is_matching_pattern(str, DATA) == 1) {
					// 	char msg[] = "503 Error: need RCPT command\n";

					// 	send(pollfds[i].fd, msg, strlen(msg), 0);
					// } else if (is_matching_pattern(str, RSET) == 1) {
					// 	char msg[] = "250 OK\n";

					// 	send(pollfds[i].fd, msg, strlen(msg), 0);
					// } else if (is_matching_pattern(str, VRFY) == 1) {
					// 	char msg[] = "502 VRFY command is disabled\n";

					// 	send(pollfds[i].fd, msg, strlen(msg), 0);
					// } else if (is_matching_pattern(str, QUIT) == 1) {
					// 	char msg[] = "221 BYE\n";

					// 	send(pollfds[i].fd, msg, strlen(msg), 0);

					// 	pollfds[i].fd = -1;
					// 	pollfds[i].events = -1;
					// 	pollfds[i].revents = -1;

					// 	current_state[i] = INIT_STATE;

					// 	useClient--;
					// 	close(pollfds[i].fd);
					// } else {
					// 	char msg[] = "502 Error: command not recognized\n";

					// 	send(pollfds[i].fd, msg, strlen(msg), 0);
					// }
				}
			}
		}

	}

	close_server_socket(socket_listen);

    return 0;
}
