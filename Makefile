CFLAGS=-Wall -Werror
EXEC=cracker
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
//make all
all: main


//make
cracker: cracker.o main.o
	gcc -o cracker cracker.o main.o
cracker.o: cracker.c
	gcc -o cracker.o -c cracker.c $(CFLAGS)
main.o: main.c cracker.h
	gcc -o main.o -c main.c $(CFLAGS)
run: cracker
	./cracker

//make tests
run: tests
	./tests

//make clean
clean:
	rm -f *.o
mrproper: clean
	rm -f $(EXEC)
