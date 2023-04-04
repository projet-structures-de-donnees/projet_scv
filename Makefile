CC = gcc


all : main

		
main.o : main.c
	$(CC) -c -o main.o -c main.c

main : main.o 
	$(CC) -o main main.o 

test.o : test.c main.c
	$(CC) -c -o test.o -c test.c

test : test.o main.o
	$(CC) -o test test.o 



clean :
	rm -f *.o main test