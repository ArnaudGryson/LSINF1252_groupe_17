main.o: main.c hello.h
	gcc -o main.o -c main.c -W -Wall -ansi -pedantic
hello: hello.o main.o
	gcc -o hello hello.o main.o
hello.o: hello.c
	gcc -o hello.o -c hello.c -W -Wall -ansi -pedantic
