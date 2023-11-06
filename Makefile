CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lm

all: build clean

build: bin/send bin/receive bin/admin

bin/send: sender.o commons.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

bin/receive: receiver.o commons.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

bin/admin: admin.o commons.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o
