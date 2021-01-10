CC=gcc
CFLAGS=-std=c99 -Wall -Werror

server: server.c ../utils/util_server.c
	$(CC) $(CFLAGS) -o server server.c ../utils/util_server.c

clean:
	rm -rf server
	