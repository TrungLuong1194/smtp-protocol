#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "../utils/util.h"

int main() {

	char hostname[MAXINPUT];
	get_input("Mail server: ", hostname);

	printf("Connecting to server: %s:25\n", hostname);

	int server = connect_to_server(hostname, "25");
	wait_on_response(server, 220);

	/* HELO */

	send_format(server, "HELO trungluong1194\r\n"); // "\r\n" = End Of Line
	wait_on_response(server, 250);

	char sender[MAXINPUT];
	get_input("From: ", sender);

	/* MAIL FROM */

	send_format(server, "MAIL FROM:<%s>\r\n", sender);
    wait_on_response(server, 250);

    char recipient[MAXINPUT];
    get_input("To: ", recipient);

    /* RCPT TO */

    send_format(server, "RCPT TO:<%s>\r\n", recipient);
    wait_on_response(server, 250);

    /* DATA */

    send_format(server, "DATA\r\n");
    wait_on_response(server, 354);

    /* 
    - Email Format:

    	From: Alice Doe <alice@example.net>
		To: Bob Doe <bob@example.com>
		Subject: Re: The Cake
		Date: Fri, 03 May 2019 02:31:20 +0000

		Do NOT forget to bring the cake!
		.
    */

    char subject[MAXINPUT];
    get_input("Subject: ", subject);

    // Header
    send_format(server, "From:<%s>\r\n", sender);
    send_format(server, "To:<%s>\r\n", recipient);
    send_format(server, "Subject:%s\r\n", subject);

    // Datetime
    time_t timer;
    time(&timer);

    struct tm *timeinfo;
    timeinfo = gmtime(&timer);

    char date[128];
    strftime(date, 128, "%a, %d %b %Y %H:%M:%S +0000", timeinfo);

    send_format(server, "Date:%s\r\n", date);
    send_format(server, "\r\n");

    printf("Enter your email text, end with \".\" on a line by itself.\n");

    // Body
    while (1) {

        char body[MAXINPUT];

        get_input("> ", body);
        send_format(server, "%s\r\n", body);

        if (strcmp(body, ".") == 0) {
            break;
        }
    }

    wait_on_response(server, 250);

    /* QUIT */

    send_format(server, "QUIT\r\n");
    wait_on_response(server, 221);

    printf("\nClosing socket...\n");
    close(server);

    printf("Finished.\n");
    return 0;
}
