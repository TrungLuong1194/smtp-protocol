CC=gcc
CFLAGS=-std=c99 -Wall -Werror

client: client.c
	$(CC) $(CFLAGS) -o client client.c

server: server.c
	$(CC) $(CFLAGS) -o server server.c

clean:
	rm -rf client server
	