#include "../utils/util.h"

int main() {

	char server[MAXINPUT];
	get_input("Mail server: ", server);

	printf("Connecting to server: %s:%s\n", server, PORT);

	int socket_peer = setup_TCP_client(server, PORT);

	printf("\n-- Begin dialog -- \n\n");
	
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
			char response[BUFSIZE + 1];
			char *p = response;
			char *end = response + BUFSIZE;

			int bytes_received = recv(socket_peer, p, end - p, 0);

			if (bytes_received < 1) {
				printf("Connection closed by host.\n");
				break;
			}

			p += bytes_received;
			*p = 0;

			if (p == end) {
	            fprintf(stderr, "Server response too large:\n");
	            fprintf(stderr, "%s", response);
	            exit(1);
	        }

			printf("%s", response);

			// Execution of QUIT cmd in server
			if (strcmp(response, "221 BYE\n") == 0) {
				printf("Connection closed by foreign host.\n");
				break;
			}
		}

		/* Send new data */

		if (pollfds[0].revents & POLLIN) { // check stdin file descriptor: 0
			char input[MAXINPUT];

			// fgets() includes the newline character from the input.
			if (!fgets(input, BUFSIZE, stdin)) { // Check ends with a newline
				break;
			}

			// int bytes_sent = send(socket_peer, read, strlen(read), 0);
			send(socket_peer, input, strlen(input), 0);
		}
	}

    close_client_socket(socket_peer);

    return 0;
}
