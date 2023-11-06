CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lm

all: build clean

build: bin/server bin/client

bin/server: server_ext.o commons_ext.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

bin/client: client_ext.o commons_ext.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o
