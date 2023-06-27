CC=gcc
CFLAGS=-g -Wall
LDLIBS=-lSDL2 -lm

all: main.c
	$(CC) $(CFLAGS) main.c -o main $(LDLIBS)

clean:
	rm -f main
