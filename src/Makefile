CC=gcc
CFLAGS=-std=c99 -Wall -Werror

client: client.c lib/utilError.c lib/utilClient.c lib/utilAddress.c
	$(CC) $(CFLAGS) -o client client.c lib/utilError.c lib/utilClient.c lib/utilAddress.c

server: server.c lib/utilError.c lib/utilServer.c lib/utilAddress.c
	$(CC) $(CFLAGS) -o server server.c lib/utilError.c lib/utilServer.c lib/utilAddress.c

clean:
	rm -rf client server
