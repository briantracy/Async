CC = gcc
CFLAGS = -std=c99
LDFLAGS = -lncurses
EXECS = client server
.PHONY: all clean

all: $(EXECS)

client: client.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

server: server.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm $(EXECS)
