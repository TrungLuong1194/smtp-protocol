#include <stdio.h>

#include "../utils/util.h"

int main() {

	char server[MAXINPUT];
	get_input("Mail server: ", server);

	printf("Connecting to server: %s:25\n", server);

	int socket_peer = setupTCPClient(server, "8080");
	
    communication_with_server(socket_peer);

    close_socket(socket_peer);

    return 0;
}
