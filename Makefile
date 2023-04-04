CC = gcc


all : main

		
main.o : main.c
	$(CC) -c -o main.o -c main.c

main : main.o 
	$(CC) -o main main.o 


clean :
	rm -f *.o main