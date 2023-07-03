IDIR=./include
ODIR=./objects

CC=gcc
CFLAGS=-g -Wall -I$(IDIR)
LDLIBS=-lSDL2 -lm

_DEPS= vector_math.h scene_structs.h raytracer.h
DEPS= $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ= main.o vector_math.o raytracer.o
OBJ= $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c $< -o $@ $(CFLAGS)

main: $(OBJ)
	$(CC) $^ -o $@ $(CFLAGS) $(LDLIBS) 

clean:
	rm -f main $(ODIR)/*.o 
