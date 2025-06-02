FLAGS = -Wall -ansi -lm -g
LDFLAGS=-lMLV -lm
LDLIBS = `pkg-config --libs-only-l MLV`

all : genetic

genetic : main.o interface.o genetic_utils.o matrix.o
	gcc -o genetic main.o interface.o genetic_utils.o matrix.o $(FLAGS) $(LDFLAGS) $(LDLIBS) 

main.o: main.c interface.h matrix.h matrix.c
	gcc -c main.c $(FLAGS) $(LDLIBS) 

interface.o: interface.c interface.h genetic_utils.h matrix.h genetic_utils.c matrix.c
	gcc -c interface.c $(FLAGS) $(LDLIBS) 

genetic_utils.o: genetic_utils.c genetic_utils.h matrix.h matrix.c
	gcc -c genetic_utils.c $(FLAGS) $(LDLIBS) 

matrix.o: matrix.c matrix.h
	gcc -c matrix.c $(FLAGS) $(LDLIBS) 

clean:
	rm -f *.o
	rm -f genetic