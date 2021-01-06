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

int main(int argc, char *argv[]) {
	
	if (argc != 3) {
		fprintf(stderr, "Usage: ./client hostname port\n");
		return 1;
	}

	/* Configure remote */

	printf("Configuring remote address...\n");

	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM; // TCP connection

	struct addrinfo *peer_address; // remote address

	if (getaddrinfo(argv[1], argv[2], &hints, &peer_address)) {
		fprintf(stderr, "getaddrinfo() failed. (%d)\n", errno);
		return 1;
	}

	/* Print information of remote */

	printf("Remote address is: ");

	char address_buffer[100];
	char service_buffer[100];

	getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen, 
		address_buffer, sizeof(address_buffer), 
		service_buffer, sizeof(service_buffer),
		NI_NUMERICHOST);

	printf("%s %s\n", address_buffer, service_buffer);

	/* Create local socket */

	printf("Creating socket...\n");

	int socket_peer;

	socket_peer = socket(peer_address->ai_family,
		peer_address->ai_socktype, peer_address->ai_protocol);

	if (socket_peer < 0) {
		fprintf(stderr, "socket() failed. (%d)\n", errno);
		return 1;
	}

	/* Connect socket */

	printf("Connecting...\n");

	if (connect(socket_peer, peer_address->ai_addr, peer_address->ai_addrlen) < 0) {
		fprintf(stderr, "connect() failed. (%d)\n", errno);
		return 1;
	}

	freeaddrinfo(peer_address);

	/* Communication */

	printf("Connected.\n");
	printf("To send data, enter text followed by enter.\n");

	while(1) {

		fd_set reads; // store socket set
		FD_ZERO(&reads); // zero-out socket set
		FD_SET(socket_peer, &reads); // add to socket set
		FD_SET(0, &reads); // add stdin to the reads

		struct timeval timeout;

		timeout.tv_sec = 0;
		timeout.tv_usec = 100000; // 100,000 microseconds

		if (select(socket_peer + 1, &reads, 0, 0, &timeout) < 0) {
			fprintf(stderr, "select() failed. (%d)\n", errno);
			return 1;
		}

		/* Receive new data */

		if (FD_ISSET(socket_peer, &reads)) { // check if a file descriptor in socket set
			char read[4096];

			int bytes_received = recv(socket_peer, read, 4096, 0);

			if (bytes_received < 1) {
				printf("Connection closed by peer.\n");
				break;
			}

			printf("\n- Received (%d bytes)\n%s", bytes_received, read);
		}

		/* Send new data */
		if (FD_ISSET(0, &reads)) { // check stdin file descriptor: 0
			char read[4096];

			// fgets() includes the newline character from the input.
			if (!fgets(read, 4096, stdin)) { // Check ends with a newline
				break;
			}

			printf("\n- Sending: %s", read);

			int bytes_sent = send(socket_peer, read, strlen(read), 0);

			printf("Sent %d bytes.\n", bytes_sent);
		}
	}

	printf("Closing socket...\n");
    close(socket_peer);

    printf("Finished.\n");
    return 0;
}
