CC=gcc
CFLAGS=-std=c99 -Wall -Werror

server: server.c ../utils/util_server.c ../utils/util_common.c ../utils/util_fsm.c
	$(CC) $(CFLAGS) -o server server.c ../utils/util_server.c ../utils/util_common.c ../utils/util_fsm.c -luuid

clean:
	rm -rf server
	