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
	logs(SERVER_LOG_DIR, INFO, "Creating socket...");

	int socket_listen;

	socket_listen = socket(bind_address->ai_family,
		bind_address->ai_socktype, bind_address->ai_protocol);
	if (socket_listen < 0) {
		fprintf(stderr, "socket() failed. (%d)\n", errno);
		logs(SERVER_LOG_DIR, ERROR, "socket() failed. (%d)", errno);
		exit(1);
	}

	// bind() associates a socket with a particular local IP address and port number
	printf("Binding socket to local address...\n");
	logs(SERVER_LOG_DIR, INFO, "Binding socket to local address...");

	if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen) < 0) {
		fprintf(stderr, "bind() failed. (%d)\n", errno);
		logs(SERVER_LOG_DIR, ERROR, "bind() failed. (%d)", errno);
		exit(1);
	}

	freeaddrinfo(bind_address);

	// listen() is used on the server to cause a TCP socket to listen for new connections
	printf("Listening...\n");
	logs(SERVER_LOG_DIR, INFO, "Listening...");

	if (listen(socket_listen, MAX_CLIENTS) < 0) {
		fprintf(stderr, "listen() failed. (%d)\n", errno);
		logs(SERVER_LOG_DIR, ERROR, "listen() failed. (%d)", errno);
		exit(1);
	}

	return socket_listen;
}

/* Close a socket */

void close_server_socket(const int socket) {

	printf("Closing listening socket...\n");
	logs(SERVER_LOG_DIR, INFO, "Closing listening socket...");
	close(socket);
	printf("Finished.\n");
	logs(SERVER_LOG_DIR, INFO, "Finished.");
}

/* Send mail when state is Ready_To_Deliver_State */

void send_mail(struct mail mc, int num_cc) {

	char *user;
	char dirname[num_cc + 1][SIZE];
	uuid_t uuid[num_cc + 1];
	char uuid_str[num_cc + 1][37];

	if (num_cc > 0) {
		// Add dirname for "cc" recipients
		for (int i = 0; i < num_cc; i++) {
			uuid_generate_random(uuid[i]);
			uuid_unparse_lower(uuid[i], uuid_str[i]);

			user = get_hostname_from_address_mail(mc.cc[i]);
			snprintf(dirname[i], sizeof dirname[i], "../../local/%s/Maildir/%s", user, uuid_str[i]);
		}
	}

	// Add dirname for "to" recipients
	uuid_generate_random(uuid[num_cc]);
	uuid_unparse_lower(uuid[num_cc], uuid_str[num_cc]);
	user = get_hostname_from_address_mail(mc.to);
	snprintf(dirname[num_cc], sizeof dirname[num_cc], "../../local/%s/Maildir/%s", user, uuid_str[num_cc]);

	for (int i = 0; i <= num_cc; i++) {
		struct tm dt = get_time();
		FILE *f;

		// printf("%s\n", dirname[i]);

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
