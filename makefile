CC = gcc
CFLAGS = -std=c99
LDFLAGS = -lncurses -pthread
EXECS = client server
.PHONY: all clean

all: $(EXECS)

client: client.c network.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

server: server.c network.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm $(EXECS)
