#include "../utils/util.h"

int main() {

	logs("../../logs/server.log", INFO, "Server start to configure.");
	int socket_listen = setup_TCP_server(PORT);

	// poll() is used to wait for an event on one or more sockets
	struct pollfd pollfds[MAX_CLIENTS + 1];

	pollfds[MASTER].fd = socket_listen;
	pollfds[MASTER].events = POLLIN | POLLPRI;

	for (int i = 1; i <= MAX_CLIENTS; i++) {
		pollfds[i].fd = INIT_VALUE; // -1 indicates available entry
	}

	int nfds = 0;

	// Create array for store state of server for each client
	int current_state[MAX_CLIENTS];

	for (int i = 1; i <= MAX_CLIENTS; i++) { // socket client start from 1 too (see in poll())
		current_state[i] = INIT_STATE;
	}

	printf("Waiting for connections...\n");
	logs("../../logs/server.log", INFO, "Waiting for connections...");

	while(TRUE) {

		char *msg; // Store reponse from server to client

		if (poll(pollfds, nfds + 1, TIME_INF) < 0) {
			fprintf(stderr, "poll() failed. (%d)\n", errno);
			logs("../../logs/server.log", ERROR, "poll() failed. (%d)", errno);
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
				logs("../../logs/server.log", ERROR, "accept() failed. (%d)", errno);
				exit(1);
			}

			// Configure new socket
			for (int i = 1; i <= MAX_CLIENTS; i++) {
				if (pollfds[i].fd < 0) {
					pollfds[i].fd = socket_client;
					pollfds[i].events = POLLIN | POLLPRI;
					nfds++;

					current_state[i] = BEGIN_STATE;
					break;
				}
			}

			// Print information of client
			char address_buffer[SIZE];

			getnameinfo((struct sockaddr*) &client_address, client_len, 
				address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
			printf("New connection from %s\n", address_buffer);
			logs("../../logs/server.log", INFO, "New connection from %s", address_buffer);

			// Send 220 code
			msg = "220 server.server Simple Mail Transfer Service Ready\n";
			send(socket_client, msg, strlen(msg), 0);
		}

		// Action from client socket
		for (int i = 1; i <= MAX_CLIENTS; i++) {

			if (pollfds[i].fd < 0) {
				continue;
			}

			if (pollfds[i].revents & POLLIN) {

				char response[BUFSIZE + 1];

				int bytes_received = recv(pollfds[i].fd, response, BUFSIZE, 0);
				if (bytes_received < 1) {
					pollfds[i].fd = INIT_VALUE;
					pollfds[i].events = INIT_VALUE;
					pollfds[i].revents = INIT_VALUE;
					nfds--;

					current_state[i] = INIT_STATE;
					close(pollfds[i].fd);
					continue;
				}

				response[bytes_received] = '\0'; // terminate the string

				/* ---------- BEGIN STATE ---------- */

				if (current_state[i] == BEGIN_STATE) {

					if (is_matching_pattern(response, HELO_CMD) == TRUE) {
						msg = "250 server.server greets client.client\n";
						send(pollfds[i].fd, msg, strlen(msg), 0);

						current_state[i] = WAIT_STATE;
					} else if (is_matching_pattern(response, EHLO_CMD) == TRUE) {
						msg = "250-server.server greets client.client\n250-VRFY\n250 OK\n";
						send(pollfds[i].fd, msg, strlen(msg), 0);

						current_state[i] = WAIT_STATE;
					} else if (is_matching_pattern(response, VRFY_CMD) == TRUE) {
						msg = "503 Error: need HELO/EHLO command\n";
						send(pollfds[i].fd, msg, strlen(msg), 0);
					} else if (is_matching_pattern(response, MAIL_CMD) == TRUE) {
						msg = "503 Error: need HELO/EHLO command\n";
						send(pollfds[i].fd, msg, strlen(msg), 0);
					} else if (is_matching_pattern(response, RCPT_CMD) == TRUE) {
						msg = "503 Error: need MAIL command\n";
						send(pollfds[i].fd, msg, strlen(msg), 0);
					} else if (is_matching_pattern(response, DATA_CMD) == TRUE) {
						msg = "503 Error: need RCPT command\n";
						send(pollfds[i].fd, msg, strlen(msg), 0);
					} else if (is_matching_pattern(response, RSET_CMD) == TRUE) {
						msg = "250 OK\n";
						send(pollfds[i].fd, msg, strlen(msg), 0);
					} else if (is_matching_pattern(response, QUIT_CMD) == TRUE) {
						msg = "221 BYE\n";
						send(pollfds[i].fd, msg, strlen(msg), 0);

						pollfds[i].fd = INIT_VALUE;
						pollfds[i].events = INIT_VALUE;
						pollfds[i].revents = INIT_VALUE;
						nfds--;

						current_state[i] = INIT_STATE;
						close(pollfds[i].fd);
					} else {
						msg = "502 Error: command not recognized\n";
						send(pollfds[i].fd, msg, strlen(msg), 0);
					}
				}

				/* ---------- WAIT STATE ---------- */

				else if (current_state[i] == WAIT_STATE) {

					if (is_matching_pattern(response, HELO_CMD) == TRUE) {
						msg = "250 server.server greets client.client\n";
						send(pollfds[i].fd, msg, strlen(msg), 0);
					} else if (is_matching_pattern(response, EHLO_CMD) == TRUE) {
						msg = "250-server.server greets client.client\n250-VRFY\n250 OK\n";
						send(pollfds[i].fd, msg, strlen(msg), 0);
					} else if (is_matching_pattern(response, VRFY_CMD) == TRUE) {
						char *hostname = get_hostname(response);
						char *res = get_address_user(hostname);

						if (strcmp(res, "No such user here") == 0) {
							char buf[BUFSIZE];

							snprintf(buf, sizeof buf, "%s%s%s", "550 ", res, "\n");
							send(pollfds[i].fd, buf, strlen(buf), 0);
						} else {
							char buf[BUFSIZE];

							snprintf(buf, sizeof buf, "%s%s%s", "250 ", res, "\n");
							send(pollfds[i].fd, buf, strlen(buf), 0);
						}
					} else if (is_matching_pattern(response, MAIL_CMD) == TRUE) {
						msg = "503 Error: need VRFY command\n";
						send(pollfds[i].fd, msg, strlen(msg), 0);
					} else if (is_matching_pattern(response, RCPT_CMD) == TRUE) {
						msg = "503 Error: need MAIL command\n";
						send(pollfds[i].fd, msg, strlen(msg), 0);
					} else if (is_matching_pattern(response, DATA_CMD) == TRUE) {
						msg = "503 Error: need RCPT command\n";
						send(pollfds[i].fd, msg, strlen(msg), 0);
					} else if (is_matching_pattern(response, RSET_CMD) == TRUE) {
						msg = "250 OK\n";
						send(pollfds[i].fd, msg, strlen(msg), 0);
					} else if (is_matching_pattern(response, QUIT_CMD) == TRUE) {
						msg = "221 BYE\n";
						send(pollfds[i].fd, msg, strlen(msg), 0);

						pollfds[i].fd = INIT_VALUE;
						pollfds[i].events = INIT_VALUE;
						pollfds[i].revents = INIT_VALUE;
						nfds--;

						current_state[i] = INIT_STATE;
						close(pollfds[i].fd);
					} else {
						msg = "502 Error: command not recognized\n";
						send(pollfds[i].fd, msg, strlen(msg), 0);
					}
				}

			}
		}
	}

	close_server_socket(socket_listen);

    return 0;
}
