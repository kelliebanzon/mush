CC = gcc  
CFLAGS = -Wall -pedantic -g -O0
LD = gcc
LDFLAGS =

all: mush

mush: mush.o main.o parseline.o
	$(LD) $(LDFLAGS) -o mush mush.o main.o parseline.o

main.o: main.c mush.h parseline.h
	$(CC) $(CFLAGS) -c -o main.o main.c

mush.o: mush.c mush.h parseline.h
	$(CC) $(CFLAGS) -c -o mush.o mush.c

parseline: parseline.o pmain.o
	$(LD) $(LDFLAGS) -o parseline parseline.o pmain.o

pmain.o: pmain.c parseline.h
	$(CC) $(CFLAGS) -c -o pmain.o pmain.c

parseline.o: parseline.c parseline.h
	$(CC) $(CFLAGS) -c -o parseline.o parseline.c


test: test_print
	echo "Done."

test_print:
	@echo "Testing..."


clean:
	rm *.o mush
