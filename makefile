CC=g++

IDIR =../include
CFLAGS=-std=gnu++11
ODIR=obj

LIBS=-lsfml-graphics -lsfml-window -lsfml-system

DEPS = config.h chip8.h

OBJ = main.o chip8.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $<  $(CFLAGS)

./chip8emu: $(OBJ)
	$(CC) -o $@ $^ $(LIBS)  $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o 
