#include "../utils/util.h"

int main() {

	char server[MAXINPUT];

	get_input("Mail server: ", server);

	printf("Connecting to server: %s:%s\n", server, PORT);

	int socket_peer = setup_TCP_client(server, PORT);

	printf("\n-- Begin dialog -- \n\n");
	
    while(TRUE) {

		struct pollfd pollfds[2];
		int nfds = 0;

		// File description for stdin
		pollfds[0].fd = 0;
		pollfds[0].events = POLLIN | POLLPRI;
		nfds++;

		// File description for socket
		pollfds[1].fd = socket_peer;
		pollfds[1].events = POLLIN | POLLPRI;
		nfds++;

		// poll() is used to wait for an event on one or more sockets
		if (poll(pollfds, nfds, TIME_INF) < 0) {
			fprintf(stderr, "poll() failed. (%d)\n", errno);
			exit(1);
		}

		// Wait events from socket
		if (pollfds[1].revents & POLLIN) {

			char response[BUFSIZE + 1];

			int bytes_received = recv(socket_peer, response, BUFSIZE, 0);

			if (bytes_received < 1) {
				printf("Connection closed by host.\n");
				break;
			}

			response[bytes_received] = '\0'; // terminate the string

			fputs(response, stdout);
		}

		// Wait events from stdin
		if (pollfds[0].revents & POLLIN) {
			
			char input[BUFSIZE];

			fgets(input, BUFSIZE, stdin);

			send(socket_peer, input, strlen(input), 0);
		}
	}

    close_client_socket(socket_peer);

    return 0;
}
