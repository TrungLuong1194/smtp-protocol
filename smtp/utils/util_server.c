#include "util.h"

/* Setup a Network Socket for server */

int setup_TCP_server(const char *port) {

	// Configure local
	printf("Configuring local address...\n");

	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo *bind_address; // local address

	getaddrinfo(0, port, &hints, &bind_address);

	// socket() creates and initializes a new socket
	printf("Creating socket...\n");
	logs("../../logs/server.log", INFO, "Creating socket...");

	int socket_listen;

	socket_listen = socket(bind_address->ai_family,
		bind_address->ai_socktype, bind_address->ai_protocol);
	if (socket_listen < 0) {
		fprintf(stderr, "socket() failed. (%d)\n", errno);
		logs("../../logs/server.log", ERROR, "socket() failed. (%d)", errno);
		exit(1);
	}

	// bind() associates a socket with a particular local IP address and port number
	printf("Binding socket to local address...\n");
	logs("../../logs/server.log", INFO, "Binding socket to local address...");

	if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen) < 0) {
		fprintf(stderr, "bind() failed. (%d)\n", errno);
		logs("../../logs/server.log", ERROR, "bind() failed. (%d)", errno);
		exit(1);
	}

	freeaddrinfo(bind_address);

	// listen() is used on the server to cause a TCP socket to listen for new connections
	printf("Listening...\n");
	logs("../../logs/server.log", INFO, "Listening...");

	if (listen(socket_listen, MAX_CLIENTS) < 0) {
		fprintf(stderr, "listen() failed. (%d)\n", errno);
		logs("../../logs/server.log", ERROR, "listen() failed. (%d)", errno);
		exit(1);
	}

	return socket_listen;
}

/* Close a socket */

void close_server_socket(const int socket) {

	printf("Closing listening socket...\n");
	logs("../../logs/server.log", INFO, "Closing listening socket...");
	close(socket);
	printf("Finished.\n");
	logs("../../logs/server.log", INFO, "Finished.");
}
