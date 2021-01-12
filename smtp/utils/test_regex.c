#include "util.h"

int main() {

	regex_t re;

	int val_regcomp, val_regexec;

	// Creation of regex
	val_regcomp = regcomp(&re, MAIL, REG_EXTENDED);

	if (val_regcomp == 0) {
		printf("Regex created successfully.\n");
	} else {
		printf("Creation error\n");
		exit(1);
	}

	// Comparing pattern
	val_regexec = regexec(&re, "maIl   FrOm : <a@a.com>", 0, NULL, 0);

	if (val_regexec == 0) {
		printf("Pattern found.\n");
	} else if (val_regexec == REG_NOMATCH) {
		printf("Pattern not found.\n");
	} else {
		printf("An error occured.\n");
	}

	return 0;
}