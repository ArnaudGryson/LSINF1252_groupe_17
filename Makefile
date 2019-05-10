CFLAGS=-Wall -Werror
EXEC=./src/cracker

all: ./src/cracker
	./src/cracker

./src/cracker:./src/cracker.o ./src/main.o
	gcc -o ./src/cracker ./src/cracker.o ./src/main.o -std=c99
./src/cracker.o:./src/cracker.c
	gcc -o ./src/cracker.o -c ./src/cracker.c -std=c99 $(CFLAGS)
./src/main.o:./src/main.c ./src/cracker.h
	gcc -o ./src/main.o -c ./src/main.c -std=c99 $(CFLAGS)

.PHONY : clean
clean:
	rm -f ./src/*.o
