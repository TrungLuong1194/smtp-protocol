CC=gcc
CFLAGS=-std=c99 -Wall -Werror

client: client.c ../utils/util_client.c
	$(CC) $(CFLAGS) -o client client.c ../utils/util_client.c

clean:
	rm -rf client
