#include "util.h"

/* Check matching of pattern in a string using regex */

int is_matching_pattern(const char *str, const char *pattern) {

	regex_t re;
	int val_regcomp, val_regexec, val_return;

	// Creation of regex
	val_regcomp = regcomp(&re, pattern, REG_EXTENDED);
	if (val_regcomp != 0) {
		fprintf(stderr, "regcomp() failed. (%s)\n", pattern);
		exit(1);
	}

	// Comparing pattern
	val_regexec = regexec(&re, str, 0, NULL, 0);
	if (val_regexec == 0) {
		val_return = TRUE;
	} else if (val_regexec == REG_NOMATCH) {
		val_return = FALSE;
	}

	return val_return;
}

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

/* Get address mail from hostname */

char *get_address_mail_from_hostname(const char *name) {

	// Initialize data for mailbox
	struct mailbox user[NUM_RECORD];

	user[0].id = 0;
	user[0].hostname = "superuser";
	user[0].address = "superuser@server.server";

	user[1].id = 1;
	user[1].hostname = "user1";
	user[1].address = "user1@server.server";

	user[2].id = 2;
	user[2].hostname = "user2";
	user[2].address = "user2@server.server";

	user[3].id = 3;
	user[3].hostname = "user3";
	user[3].address = "user3@server.server";

	user[4].id = 4;
	user[4].hostname = "user4";
	user[4].address = "user4@server.server";

	user[5].id = 5;
	user[5].hostname = "user5";
	user[5].address = "user5@server.server";

	user[6].id = 6;
	user[6].hostname = "user6";
	user[6].address = "user6@server.server";

	user[7].id = 7;
	user[7].hostname = "user7";
	user[7].address = "user7@server.server";

	user[8].id = 8;
	user[8].hostname = "user8";
	user[8].address = "user8@server.server";

	user[9].id = 9;
	user[9].hostname = "user9";
	user[9].address = "user9@server.server";

	// Check mailbox record
	char *addr;

	for (int i = 0; i < NUM_RECORD; i++) {
		if (strcmp(name, user[i].hostname) == 0) {
			addr = user[i].address;

			return addr;
		}
	}

	addr = "No such user here";

	return addr;
}

/* Get hostname from VRFY command */

char *get_hostname_from_vrfy(char* input) {

	// New string except first 4 letter 'vrfy' and last '\n'
	char tmp[strlen(input) - 5];

	for (int i = 0; i < strlen(input) - 5; i++) {
		tmp[i] = input[i + 4];
	}

	// Delete spaces
	char *output = (char*) malloc(strlen(tmp));
	char *dest = output;

	if (output) {
		for (int i = 0; i < strlen(tmp); i++) {
			if (tmp[i] != ' ') {
				*dest++ = tmp[i];
			}
		}

		*dest = '\0';
	}

	return output;
}

/* Get address mail from RCPT command */

void get_address_from_rcpt(char *input, char *output) {
	
	int start, end;
	
	for (int i = 0; i < strlen(input); i++) {
		if (input[i] == '<') {
			start = i;
		} else if (input[i] == '>') {
			end = i;
		}
	}

	int index = 0;
	
	for (int i = start + 1; i < end; i++) {
		output[index] = input[i];
		index++;
	}
	
	output[index] = '\0';
}

/* Check address mail in mailbox? */

int check_address_in_mailbox(char *mail) {

	// Initialize data for mailbox
	struct mailbox user[NUM_RECORD];

	user[0].id = 0;
	user[0].hostname = "superuser";
	user[0].address = "superuser@server.server";

	user[1].id = 1;
	user[1].hostname = "user1";
	user[1].address = "user1@server.server";

	user[2].id = 2;
	user[2].hostname = "user2";
	user[2].address = "user2@server.server";

	user[3].id = 3;
	user[3].hostname = "user3";
	user[3].address = "user3@server.server";

	user[4].id = 4;
	user[4].hostname = "user4";
	user[4].address = "user4@server.server";

	user[5].id = 5;
	user[5].hostname = "user5";
	user[5].address = "user5@server.server";

	user[6].id = 6;
	user[6].hostname = "user6";
	user[6].address = "user6@server.server";

	user[7].id = 7;
	user[7].hostname = "user7";
	user[7].address = "user7@server.server";

	user[8].id = 8;
	user[8].hostname = "user8";
	user[8].address = "user8@server.server";

	user[9].id = 9;
	user[9].hostname = "user9";
	user[9].address = "user9@server.server";

	int result = FALSE;

	for (int i = 0; i < NUM_RECORD; i++) {
		if (strcmp(mail, user[i].address) == 0) {
			result = TRUE;

			break;
		}
	}

	return result;
}