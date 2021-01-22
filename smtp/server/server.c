#include "../utils/util.h"

int main() {

	logs(SERVER_LOG_DIR, INFO, "Server start to configure.");
	int socket_listen = setup_TCP_server(PORT);

	// Initial data for mailbox
	struct mailbox *user = init_mailbox();

	printf("List mailbox:\n");
	for (int i = 0; i < NUM_RECORD; i++) {
		printf("- %s\n", user[i].hostname);
	}

	// poll() is used to wait for an event on one or more sockets
	struct pollfd pollfds[MAX_CLIENTS + 1];

	pollfds[MASTER].fd = socket_listen;
	pollfds[MASTER].events = POLLIN | POLLPRI;

	for (int i = 1; i <= MAX_CLIENTS; i++) {
		pollfds[i].fd = INIT_VALUE; // -1 indicates available entry
	}

	int nfds = 0;

	// Create array for store state of server for each client
	eSystemState eCurrentState[MAX_CLIENTS];

	for (int i = 1; i <= MAX_CLIENTS; i++) { // socket client start from 1 too (see in poll())
		eCurrentState[i] = Init_State;
	}

	// Initialize Mail Content
	struct mail mailContent[MAX_CLIENTS];

	int num_cc[MAX_CLIENTS]; // Number of recipients cc

	printf("Waiting for connections...\n");
	logs(SERVER_LOG_DIR, INFO, "Waiting for connections...");

	while(TRUE) {

		if (poll(pollfds, nfds + 1, TIME_INF) < 0) {
			fprintf(stderr, "poll() failed. (%d)\n", errno);
			logs(SERVER_LOG_DIR, ERROR, "poll() failed. (%d)", errno);
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
				logs(SERVER_LOG_DIR, ERROR, "accept() failed. (%d)", errno);
				exit(1);
			}

			// Configure new socket
			for (int i = 1; i <= MAX_CLIENTS; i++) {
				if (pollfds[i].fd < 0) {
					pollfds[i].fd = socket_client;
					pollfds[i].events = POLLIN | POLLPRI;
					nfds++;

					eCurrentState[i] = Begin_State;
					num_cc[i] = 0;

					break;
				}
			}

			// Print information of client
			char address_buffer[SIZE];

			getnameinfo((struct sockaddr*) &client_address, client_len, 
				address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
			printf("New connection from %s\n", address_buffer);
			logs(SERVER_LOG_DIR, INFO, "New connection from %s", address_buffer);

			// Send 220 code
			char *msg;

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

					eCurrentState[i] = Init_State;
					close(pollfds[i].fd);
					continue;
				}

				response[bytes_received] = '\0'; // terminate the string

				eSystemEvent eNewEvent = event_trigger(response);
				eSystemState tmp = eCurrentState[i];
				eCurrentState[i] = fsm_state_handler(&pollfds[i], &nfds, response, eCurrentState[i], eNewEvent, &mailContent[i], &num_cc[i]);

				// Create maildir for each recipient
				if (tmp == Writing_Mail_State && eCurrentState[i] == Ready_To_Deliver_State) {
					send_mail(mailContent[i], num_cc[i]);
				}
			}
		}
	}

	close_server_socket(socket_listen);

    return 0;
}
