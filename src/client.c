#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "lib/utilLib.h"

int main(int argc, char *argv[]) {
	
	if (argc != 4) {
		errorUser("Parameter(s)", "<Server Address/Name> <Echo Word> <Server Port/Service>");
	}

	char *server = argv[1]; // First arg: server address/name
	char *echoString = argv[2]; // Second arg: string to echo
	char *service = argv[3]; // Third arg: server port (numeric)/service

	// Create a TCP socket
	int sock = setupTCPClientSocket(server, service);

	if (sock < 0) {
		errorUser("setupTCPClientSocket() failed", "unable to connect");
	}

	size_t echoStringLen = strlen(echoString); // Determine input length

	// Send the string to the server
	ssize_t numBytes = send(sock, echoString, echoStringLen, 0);

	if (numBytes < 0) {
		errorSystem("send() failed");
	} else if (numBytes != echoStringLen) {
		errorUser("send()", "sent unexpected number of bytes");
	}

	// Receive the same string back from the server
	unsigned int totalBytesRcvd = 0; // Count of total bytes received
	fputs("Received: ", stdout); // Setup to print the echoed string

	while (totalBytesRcvd < echoStringLen) {
		char buffer[BUFSIZE]; // I/O buffer

		numBytes = recv(sock, buffer, BUFSIZE - 1, 0);

		if (numBytes < 0) {
			errorSystem("recv() failed");
		} else if (numBytes == 0) {
			errorUser("recv()", "connection closed prematurely");
		}

		totalBytesRcvd += numBytes;
		buffer[numBytes] = '\0'; // Terminate the string
		fputs(buffer, stdout);
	}

	fputc('\n', stdout);

	close(sock);

	return 0;
}
