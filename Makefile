CC = gcc  
CFLAGS = -Wall -pedantic -ansi -g -O0
LD = gcc
LDFLAGS =

all: parseline


parseline: parseline.o main.o
	$(LD) $(LDFLAGS) -o parseline parseline.o main.o

main.o: main.c parseline.h
	$(CC) $(CFLAGS) -c -o main.o main.c

parseline.o: parseline.c parseline.h
	$(CC) $(CFLAGS) -c -o parseline.o parseline.c


test: test_print
	echo "Done."

test_print:
	@echo "Testing..."


clean:
	rm *.o
