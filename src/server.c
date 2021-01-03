#include <stdio.h>
#include <unistd.h>
#include "lib/utilLib.h"

int main(int argc, char *argv[]) {

	if (argc != 2) {
		errorUser("Parameter(s)", "<Server Port/Service>");
	}

	char *service = argv[1]; // First arg: local port

	// Create socket for incoming connections
	int servSock = setupTCPServerSocket(service);

	if (servSock < 0) {
		errorUser("setupTCPServerSocket() failed", service);
	}

	// Unlimited iterations
	for (;;) {
		// New connection creates a connected client socket
		int clntSock = acceptTCPConnection(servSock);

		handleTCPClient(clntSock);
		close(clntSock);
	}

	// Not reached
	close(servSock);
}
