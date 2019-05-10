CFLAGS=-Wall -Werror
EXEC=src/cracker

all: src/cracker
	./src/cracker
src/cracker:src/cracker.o src/reverse.o src/SHA256.o
	gcc -o src/cracker src/cracker.o src/reverse.o src/SHA256.o -std=c99

src/reverse.o:src/reverse.c src/reverse.h
	gcc -o src/reverse.o -c src/reverse.c -std=c99 $(CFLAGS)
src/SHA256.o:src/SHA256.c src/SHA256.h
	gcc -o src/SHA256.o -c src/SHA256.c -std=c99 $(CFLAGS)

src/cracker.o:src/cracker.c src/cracker.h
	gcc -o src/cracker.o -c src/cracker.c -std=c99 $(CFLAGS)
src/main.o:src/main.c src/main.h
	gcc -o src/main.o -c src/main.c -std=c99 $(CFLAGS)

.PHONY : clean
clean:
	rm -f ./src/*.o
