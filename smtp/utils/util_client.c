#include "util.h"

/* Setup a Network Socket for client */

int setup_TCP_client(const char *server, const char *port) {

	// Configure remote
	printf("Configuring remote address...\n");

	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM; // TCP connection

	struct addrinfo *peer_address; // remote address

	if (getaddrinfo(server, port, &hints, &peer_address)) {
		fprintf(stderr, "getaddrinfo() failed. (%d)\n", errno);
		exit(1);
	}

	// Print information of remote
	printf("Remote address is: ");

	char address_buffer[SIZE];
	char service_buffer[SIZE];

	getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen, 
		address_buffer, sizeof(address_buffer), 
		service_buffer, sizeof(service_buffer),
		NI_NUMERICHOST);
	printf("%s %s\n", address_buffer, service_buffer);

	// socket() creates and initializes a new socket
	printf("Creating socket...\n");
	logs(CLIENT_LOG_DIR, INFO, "Creating socket...");

	int socket_peer;

	socket_peer = socket(peer_address->ai_family,
		peer_address->ai_socktype, peer_address->ai_protocol);
	if (socket_peer < 0) {
		fprintf(stderr, "socket() failed. (%d)\n", errno);
		logs(CLIENT_LOG_DIR, ERROR, "socket() failed. (%d)", errno);
		exit(1);
	}

	// connect() is used on the client to set the remote address and port
	printf("Connecting...\n");
	logs(CLIENT_LOG_DIR, INFO, "Connecting...");

	if (connect(socket_peer, peer_address->ai_addr, peer_address->ai_addrlen) < 0) {
		fprintf(stderr, "connect() failed. (%d)\n", errno);
		logs(CLIENT_LOG_DIR, ERROR, "connect() failed. (%d)", errno);
		exit(1);
	}

	freeaddrinfo(peer_address);
	printf("Connected.\n");
	logs(CLIENT_LOG_DIR, INFO, "Connected.");

    return socket_peer;
}

/* Get data from input stdin ( equivalent command gets() )*/

void get_input(const char *prompt, char *buffer) {

	printf("%s", prompt);
	fgets(buffer, MAXINPUT, stdin);

	int len = strlen(buffer);
	if (len > 0) {
		buffer[len - 1] = 0; // 0 = null
	}
}

/* Close a socket */

void close_client_socket(const int socket) {

	printf("Closing socket...\n");
	logs(CLIENT_LOG_DIR, INFO, "Closing socket...");
	close(socket);
	printf("Finished.\n");
	logs(CLIENT_LOG_DIR, INFO, "Finished.");
}
