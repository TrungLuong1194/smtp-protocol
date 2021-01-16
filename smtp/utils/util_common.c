#include "util.h"

/* Get current datetime */

struct tm get_time() {

	time_t t = time(NULL);
	struct tm current_datetime = *localtime(&t);
	
	return current_datetime;
}

/* Write log connection history on client and server */

void logs(const char *filename, const char *level, const char *text, ...) {

	struct tm dt = get_time();
	char msg[LOGSIZE];
	FILE *f;

	f = fopen(filename, "a");
	if (f == NULL) {
		printf("Unable to create file.\n");
		exit(1);
	}

	va_list args;
	va_start(args, text);
	vsprintf(msg, text, args);
	va_end(args);

	fprintf(f, "%d-%02d-%02d %02d:%02d:%02d - LOGLEVEL:%s - MESSAGE:%s\n", 
		dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec, level, msg);
	fclose(f);
}