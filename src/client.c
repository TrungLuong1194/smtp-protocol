#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "utility.h"

int main(int argc, char *argv[]) {
	if (argc != 4) {
		errorUser("Parameter(s)", "<Server Address> <Echo Word> <Server Port>");
	}

	char *servIP = argv[1]; // First arg: server IP address
	char *echoString = argv[2]; // Second arg: string to echo

	in_port_t servPort = atoi(argv[3]); // Third arg: server port (numeric)

	// Create a reliable, stream socket using TCP
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sock < 0) {
		errorSystem("socket() failed");
	}

	// Construct the server address structure
	struct sockaddr_in servAddr; // Server address
	memset(&servAddr, 0, sizeof(servAddr)); // Zero out structure
	servAddr.sin_family = AF_INET; // IPv4 address family

	// Convert address: convert IPv4 and IPv6 addresses from text to binary form
	int val = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);

	if (val == 0) {
		errorUser("inet_pton() failed", "invalid address string");
	} else if (val < 0) {
		errorSystem("inet_pton() failed");
	}

	servAddr.sin_port = htons(servPort); // Server port

	// Establish the connection to the echo server
	if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
		errorSystem("connect() failed");
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
