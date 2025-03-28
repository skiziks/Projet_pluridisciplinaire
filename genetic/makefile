FLAGS = -Wall -ansi -lm
LDFLAGS=-lMLV -lm
LDLIBS = `pkg-config --libs-only-l MLV`

all : main

genetic : main.o interface.o
	gcc -o genetic main.o interface.o $(FLAGS) $(LDFLAGS)

main.o: main.c interface.h
	gcc -c main.c $(FLAGS)

interface.o: interface.c interface.h
	gcc -c interface.c $(FLAGS)