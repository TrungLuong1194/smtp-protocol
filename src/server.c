#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "utilLib.h"

int main(int argc, char *argv[]) {

	if (argc != 2)
		errorUser("Parameter(s)", "<Server Port>");

	in_port_t servPort = atoi(argv[1]); // First arg: local port

	// Create socket for incoming connections
	int servSock; // Socket descriptor for server

	if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		errorSystem("socket() failed");
	}

	// Construct local address structure
	struct sockaddr_in servAddr; // Local address
	memset(&servAddr, 0, sizeof(servAddr)); // Zero out structure
	servAddr.sin_family = AF_INET; // IPv4 address family
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface
	servAddr.sin_port = htons(servPort); // Local port

	// Bind to the local address
	if (bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
		errorSystem("bind() failed");
	}

	// Mark the socket so it will listen for incoming connections
	if (listen(servSock, MAXPENDING) < 0) {
		errorSystem("listen() failed");
	}

	// Unlimited iterations
	for (;;) {
		struct sockaddr_in clntAddr; // Client address

		// Set length of client address structure
		socklen_t clntAddrLen = sizeof(clntAddr);

		// Wait for a client to connect
		int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);

		if (clntSock < 0) {
			errorSystem("accept() failed");
		}

		// clntSock is connected to a client
		char clntName[INET_ADDRSTRLEN]; // String to contain client address

		if (inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr, clntName, sizeof(clntName)) != NULL) {
			printf("Handling client %s/%d\n", clntName, ntohs(clntAddr.sin_port));
		} else {
			puts("Unable to get client address");
		}

		HandleTCPClient(clntSock);
	}

	// Not reached
}