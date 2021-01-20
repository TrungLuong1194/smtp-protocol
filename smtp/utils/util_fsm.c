#include "util.h"

/* Event Trigger */

eSystemEvent event_trigger(char *response) {

	eSystemEvent eNewEvent;

	if (is_matching_pattern(response, HELO_CMD) == TRUE) {
		eNewEvent = Helo_Event;
	} else if (is_matching_pattern(response, EHLO_CMD) == TRUE) {
		eNewEvent = Ehlo_Event;
	} else if (is_matching_pattern(response, VRFY_CMD) == TRUE) {
		eNewEvent = Vrfy_Event;
	} else if (is_matching_pattern(response, MAIL_CMD) == TRUE) {
		eNewEvent = Mail_Event;
	} else if (is_matching_pattern(response, RCPT_CMD) == TRUE) {
		eNewEvent = Rcpt_Event;
	} else if (is_matching_pattern(response, DATA_CMD) == TRUE) {
		eNewEvent = Data_Event;
	} else if (is_matching_pattern(response, RSET_CMD) == TRUE) {
		eNewEvent = Rset_Event;
	} else if (is_matching_pattern(response, QUIT_CMD) == TRUE) {
		eNewEvent = Quit_Event;
	} else {
		eNewEvent = Undefined_Event;
	}

	return eNewEvent;
}

/* Handler event in each State */

eSystemState fsm_state_handler(struct pollfd *pfds, int *nfds, char *response, char *body, eSystemState eCurrentState, eSystemEvent eNewEvent) {

	char *msg;
	eSystemState eNextState = eCurrentState;

	switch(eCurrentState) {

		case Begin_State:

			if (eNewEvent == Helo_Event) {
				msg = "250 server.server greets client.client\n";
				send(pfds->fd, msg, strlen(msg), 0);

				eNextState = Wait_State;
			} else if (eNewEvent == Ehlo_Event) {
				msg = "250-server.server greets client.client\n250-VRFY\n250 OK\n";
				send(pfds->fd, msg, strlen(msg), 0);

				eNextState = Wait_State;
			} else if (eNewEvent == Vrfy_Event) {
				msg = "503 Error: need HELO/EHLO command\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Mail_Event) {
				msg = "503 Error: need HELO/EHLO command\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Rcpt_Event) {
				msg = "503 Error: need MAIL command\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Data_Event) {
				msg = "503 Error: need RCPT command\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Rset_Event) {
				msg = "503 Error: need HELO/EHLO command\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Quit_Event) {
				msg = "221 BYE\n";
				send(pfds->fd, msg, strlen(msg), 0);

				pfds->fd = INIT_VALUE;
				pfds->events = INIT_VALUE;
				pfds->revents = INIT_VALUE;
				(*nfds)--;

				close(pfds->fd);
			} else if (eNewEvent == Undefined_Event) {
				msg = "502 Error: command not recognized\n";
				send(pfds->fd, msg, strlen(msg), 0);
			}

			break;

		case Wait_State:

			if (eNewEvent == Helo_Event) {
				msg = "503 Error: need MAIL/VRFY command\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Ehlo_Event) {
				msg = "503 Error: need MAIL/VRFY command\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Vrfy_Event) {
				char *hostname = get_hostname(response);
				char *res = get_address_user(hostname);

				if (strcmp(res, "No such user here") == 0) {
					char buf[BUFSIZE];

					snprintf(buf, sizeof buf, "%s%s%s", "550 ", res, "\n");
					send(pfds->fd, buf, strlen(buf), 0);
				} else {
					char buf[BUFSIZE];

					snprintf(buf, sizeof buf, "%s%s%s", "250 ", res, "\n");
					send(pfds->fd, buf, strlen(buf), 0);
				}
			} else if (eNewEvent == Mail_Event) {
				msg = "250 OK\n";
				send(pfds->fd, msg, strlen(msg), 0);

				eNextState = Envelope_Create_State;
			} else if (eNewEvent == Rcpt_Event) {
				msg = "503 Error: need MAIL command\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Data_Event) {
				msg = "503 Error: need RCPT command\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Rset_Event) {
				msg = "250 OK\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Quit_Event) {
				msg = "221 BYE\n";
				send(pfds->fd, msg, strlen(msg), 0);

				pfds->fd = INIT_VALUE;
				pfds->events = INIT_VALUE;
				pfds->revents = INIT_VALUE;
				(*nfds)--;

				close(pfds->fd);
			} else if (eNewEvent == Undefined_Event) {
				msg = "502 Error: command not recognized\n";
				send(pfds->fd, msg, strlen(msg), 0);
			}

			break;

		case Envelope_Create_State:

			if (eNewEvent == Helo_Event) {
				msg = "503 Error: need RCPT/VRFY command\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Ehlo_Event) {
				msg = "503 Error: need RCPT/VRFY command\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Vrfy_Event) {
				char *hostname = get_hostname(response);
				char *res = get_address_user(hostname);

				if (strcmp(res, "No such user here") == 0) {
					char buf[BUFSIZE];

					snprintf(buf, sizeof buf, "%s%s%s", "550 ", res, "\n");
					send(pfds->fd, buf, strlen(buf), 0);
				} else {
					char buf[BUFSIZE];

					snprintf(buf, sizeof buf, "%s%s%s", "250 ", res, "\n");
					send(pfds->fd, buf, strlen(buf), 0);
				}
			} else if (eNewEvent == Mail_Event) {
				msg = "503 Error: need RCPT/VRFY command\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Rcpt_Event) {
				msg = "250 OK\n";
				send(pfds->fd, msg, strlen(msg), 0);

				eNextState = Recipients_Set_State;
			} else if (eNewEvent == Data_Event) {
				msg = "503 Error: need RCPT command\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Rset_Event) {
				msg = "250 OK\n";
				send(pfds->fd, msg, strlen(msg), 0);

				eNextState = Wait_State;
			} else if (eNewEvent == Quit_Event) {
				msg = "221 BYE\n";
				send(pfds->fd, msg, strlen(msg), 0);

				pfds->fd = INIT_VALUE;
				pfds->events = INIT_VALUE;
				pfds->revents = INIT_VALUE;
				(*nfds)--;

				close(pfds->fd);
			} else if (eNewEvent == Undefined_Event) {
				msg = "502 Error: command not recognized\n";
				send(pfds->fd, msg, strlen(msg), 0);
			}

			break;

		case Recipients_Set_State:

			if (eNewEvent == Helo_Event) {
				msg = "503 Error: need RCPT/VRFY command\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Ehlo_Event) {
				msg = "503 Error: need RCPT/VRFY command\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Vrfy_Event) {
				char *hostname = get_hostname(response);
				char *res = get_address_user(hostname);

				if (strcmp(res, "No such user here") == 0) {
					char buf[BUFSIZE];

					snprintf(buf, sizeof buf, "%s%s%s", "550 ", res, "\n");
					send(pfds->fd, buf, strlen(buf), 0);
				} else {
					char buf[BUFSIZE];

					snprintf(buf, sizeof buf, "%s%s%s", "250 ", res, "\n");
					send(pfds->fd, buf, strlen(buf), 0);
				}
			} else if (eNewEvent == Mail_Event) {
				msg = "503 Error: need RCPT command\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Rcpt_Event) {
				msg = "250 OK\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Data_Event) {
				msg = "354 Start mail input; end with <CRLF>.<CRLF>\n";
				send(pfds->fd, msg, strlen(msg), 0);

				eNextState = Writing_Mail_State;
			} else if (eNewEvent == Rset_Event) {
				msg = "250 OK\n";
				send(pfds->fd, msg, strlen(msg), 0);

				eNextState = Wait_State;
			} else if (eNewEvent == Quit_Event) {
				msg = "221 BYE\n";
				send(pfds->fd, msg, strlen(msg), 0);

				pfds->fd = INIT_VALUE;
				pfds->events = INIT_VALUE;
				pfds->revents = INIT_VALUE;
				(*nfds)--;

				close(pfds->fd);
			} else if (eNewEvent == Undefined_Event) {
				msg = "502 Error: command not recognized\n";
				send(pfds->fd, msg, strlen(msg), 0);
			}

			break;

		case Writing_Mail_State:

			if (strcmp(response, ".\n") != 0) {
				strcat(body, response);
			} else {
				msg = "250 OK\n";
				send(pfds->fd, msg, strlen(msg), 0);

				printf("Body Content:\n%s", body);

				eNextState = Ready_To_Deliver_State;
			}

			break;

		case Ready_To_Deliver_State:

			if (eNewEvent == Helo_Event) {
				msg = "503 Error: need MAIL/VRFY command\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Ehlo_Event) {
				msg = "503 Error: need MAIL/VRFY command\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Vrfy_Event) {
				char *hostname = get_hostname(response);
				char *res = get_address_user(hostname);

				if (strcmp(res, "No such user here") == 0) {
					char buf[BUFSIZE];

					snprintf(buf, sizeof buf, "%s%s%s", "550 ", res, "\n");
					send(pfds->fd, buf, strlen(buf), 0);
				} else {
					char buf[BUFSIZE];

					snprintf(buf, sizeof buf, "%s%s%s", "250 ", res, "\n");
					send(pfds->fd, buf, strlen(buf), 0);
				}
			} else if (eNewEvent == Mail_Event) {
				msg = "250 OK\n";
				send(pfds->fd, msg, strlen(msg), 0);

				eNextState = Envelope_Create_State;
			} else if (eNewEvent == Rcpt_Event) {
				msg = "503 Error: need MAIL command\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Data_Event) {
				msg = "503 Error: need RCPT command\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Rset_Event) {
				msg = "503 Error: need MAIL command\n";
				send(pfds->fd, msg, strlen(msg), 0);
			} else if (eNewEvent == Quit_Event) {
				msg = "221 BYE\n";
				send(pfds->fd, msg, strlen(msg), 0);

				pfds->fd = INIT_VALUE;
				pfds->events = INIT_VALUE;
				pfds->revents = INIT_VALUE;
				(*nfds)--;

				close(pfds->fd);
			} else if (eNewEvent == Undefined_Event) {
				msg = "502 Error: command not recognized\n";
				send(pfds->fd, msg, strlen(msg), 0);
			}

			break;

		default:
			break;
	}
	
	return eNextState;
}
