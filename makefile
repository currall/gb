# Makefile

CC = gcc

all:
	$(CC) main.c -o main

clean:
	rm -f *.exe
