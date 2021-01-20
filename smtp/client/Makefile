CC=gcc
CFLAGS=-std=c99 -Wall -Werror

client: client.c ../utils/util_client.c ../utils/util_common.c
	$(CC) $(CFLAGS) -o client client.c ../utils/util_client.c ../utils/util_common.c

clean:
	rm -rf client
