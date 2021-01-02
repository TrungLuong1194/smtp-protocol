CC=gcc
CFLAGS=-std=c99 -Wall -Werror

client: client.c errorMsg.c
		$(CC) $(CFLAGS) -o client client.c errorMsg.c

clean:
		rm -rf client
