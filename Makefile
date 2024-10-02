CFLAGS=-std=c11 -Wall -Og -g -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc
CC=gcc


all: lab

lab: lab.o util.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f lab.o util.o lab

.PHONY: all clean
