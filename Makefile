CC=gcc
CFLAGS=-g -Wall -I.
LDLIBS=-lSDL2 -lm
DEPS= vector_math.h scene_structs.h
OBJ= main.o vector_math.o 

%.o: %.c $(DEPS)
	$(CC) -c $< -o $@ $(CFLAGS)

main: $(OBJ)
	$(CC) $^ -o $@ $(CFLAGS) $(LDLIBS) 

clean:
	rm -f main
