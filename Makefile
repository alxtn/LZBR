IDIR =./
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj

LIBS=-lm -lstdc++

_DEPS = dict.h comp.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = dict.o comp.o LZBR.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

LZBR: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)
