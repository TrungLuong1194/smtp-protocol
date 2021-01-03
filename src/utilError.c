#include <stdio.h>
#include <stdlib.h>

void errorUser(char *msg, char *detail) {
	fputs(msg, stderr);
	fputs(": ", stderr);
	fputs(detail, stderr);
	fputc('\n', stderr);
	exit(1);
}

void errorSystem(char *msg) {
	perror(msg);
	exit(1);
}
