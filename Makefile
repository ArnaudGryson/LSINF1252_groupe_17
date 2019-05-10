CFLAGS=-Wall -Werror
EXEC=src/cracker

all: src/cracker
	./src/cracker src/intput/01_4c_1k.bin
src/cracker:src/cracker.o src/reverse.o src/sha256.o
	gcc -o src/cracker src/cracker.o src/reverse.o src/sha256.o -lpthread -std=c99

src/reverse.o:src/reverse.c src/reverse.h
	gcc -o src/reverse.o -c src/reverse.c -std=c99 $(CFLAGS)
src/sha256.o:src/sha256.c src/sha256.h
	gcc -o src/sha256.o -c src/sha256.c -std=c99 $(CFLAGS)

src/cracker.o:src/cracker.c src/cracker.h
	gcc -o src/cracker.o -c src/cracker.c -std=c99 $(CFLAGS)

.PHONY : clean
clean:
	rm -f ./src/*.o
