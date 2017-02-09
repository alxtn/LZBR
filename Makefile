IDIR =./
CC=g++
CFLAGS=-I$(IDIR)

ODIR=obj

LIBS=-lm -lstdc++

_DEPS = dict.h comp.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = dict.o comp.o LZBR.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)

LZBR: $(OBJ)
	$(CC) -g -o $@ $^ $(CFLAGS) $(LIBS)
