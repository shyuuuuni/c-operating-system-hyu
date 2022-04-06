
# file    Makefile
# usage		$ make : Make Executable
#					$ ./sudoku-test : Execute sudoku validator program.
#					$ make clean : Clear object files and Executable
# author  Seunghyun Kim
CC=gcc
CFLAGS=-g -Wall
OBJS=sudoku-solution-validator.o
TARGET=sudoku-test

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

sudoku-solution-validator.o: sudoku-solution-validator.c
	gcc -c sudoku-solution-validator.c

clean:
	rm -f *.o
	rm -f *.out
	rm -f $(TARGET)