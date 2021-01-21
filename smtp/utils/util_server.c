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

/* Send mail when state is Ready_To_Deliver_State */

void send_mail(struct mail mc, int num_cc) {

	char *user;
	char dirname[num_cc + 1][SIZE];

	if (num_cc > 0) {
		// Add dirname for "cc" recipients
		for (int i = 0; i < num_cc; i++) {
			user = get_hostname_from_address_mail(mc.cc[i]);
			snprintf(dirname[i], sizeof dirname[i], "../../local/%s/Maildir/mail.txt", user);
		}
	}

	// Add dirname for "to" recipients
	user = get_hostname_from_address_mail(mc.to);
	snprintf(dirname[num_cc], sizeof dirname[num_cc], "../../local/%s/Maildir/mail.txt", user);

	for (int i = 0; i <= num_cc; i++) {
		struct tm dt = get_time();
		FILE *f;

		printf("%s\n", dirname[i]);

		f = fopen(dirname[i], "w");
		if (f == NULL) {
			printf("Unable to create file.\n");
			exit(1);
		}

		fprintf(f, "FROM: <%s>\n", mc.from);
		fprintf(f, "TO: <%s>\n", mc.to);

		if (num_cc > 0) {
			fprintf(f, "CC:");

			for (int i = 0; i < num_cc; i++) {
				if (i != 0) {
					fprintf(f, ", <%s>", mc.cc[i]);
				} else {
					fprintf(f, " <%s>", mc.cc[i]);
				}
			}

			fprintf(f, "\n");
		}

		fprintf(f, "DATE: %d-%02d-%02d %02d:%02d:%02d\n\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
		fprintf(f, "%s", mc.body);
		fclose(f);
	}
}
