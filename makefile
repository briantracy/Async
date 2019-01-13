
MAKEFLAGS += --warn-undefined-variables
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lncurses -pthread

CLIENT_NAME = async-client
SERVER_NAME = async-server

EXECS = client server

.PHONY: all clean

all: client server

client: client.c network.c
	$(CC) $(CFLAGS) -D PROGNAME=\"$(CLIENT_NAME)\" $(LDFLAGS) $^ -o $@

server: server.c network.c
	$(CC) $(CFLAGS) -D PROGNAME=\"$(SERVER_NAME)\" $(LDFLAGS) $^ -o $@

clean:
	rm $(EXECS)

