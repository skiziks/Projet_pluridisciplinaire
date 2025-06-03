FLAGS = -Wall -lm -g
LDFLAGS=-lMLV -lm
LDLIBS = `pkg-config --libs-only-l MLV`

all : genetic

genetic : main.o interface.o genetic_utils.o matrix.o input.o output.o
	gcc -o genetic main.o interface.o genetic_utils.o matrix.o input.o output.o $(FLAGS) $(LDFLAGS) $(LDLIBS) 

main.o: main.c interface.h matrix.h matrix.c input.h genetic_utils.h input.c output.h output.c
	gcc -c main.c $(FLAGS) $(LDLIBS) 

interface.o: interface.c interface.h genetic_utils.h matrix.h genetic_utils.c matrix.c
	gcc -c interface.c $(FLAGS) $(LDLIBS) 

genetic_utils.o: genetic_utils.c genetic_utils.h matrix.h matrix.c
	gcc -c genetic_utils.c $(FLAGS) $(LDLIBS) 

matrix.o: matrix.c matrix.h
	gcc -c matrix.c $(FLAGS) $(LDLIBS) 

input.o: input.c input.h
	gcc -c input.c $(FLAGS) $(LDLIBS)

output.o: output.c output.h
	gcc -c output.c $(FLAGS) $(LDLIBS)

clean:
	rm -f *.o
	rm -f genetic