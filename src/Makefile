CC=gcc
CFLAGS=-std=c99 -Wall -Werror

client: client.c utilError.c
	$(CC) $(CFLAGS) -o client client.c utilError.c

server: server.c utilError.c utilServer.c
	$(CC) $(CFLAGS) -o server server.c utilError.c utilServer.c

clean:
	rm -rf client server
