
# file    Makefile
# usage		$ make : Make Executable
#					$ ./simple_shell : Execute shell program.
#					$ make clean : Clear object files and Executable
# author  Seunghyun Kim
CC=gcc
CFLAGS=-g -Wall
OBJS=simple_shell.o
TARGET=simple_shell

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

simple_shell.o: simple_shell.c
	gcc -c simple_shell.c

clean:
	rm -f *.o
	rm -f *.test
	rm -f $(TARGET)