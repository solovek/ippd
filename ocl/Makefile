CC  = gcc
SRC = $(shell find . -name "*.c") ../util.c
OBJ = $(addsuffix .o,$(basename $(SRC)))

DEP = OpenCL

CFLAG = `pkg-config $(DEP) --cflags`
LFLAG = `pkg-config $(DEP) --libs`

EXE = mult

.SUFFIX: .c .o
.PHONY: all test clean

all: $(EXE) test

test: $(EXE)
	./$(EXE) mult
	./$(EXE) trap

clean:
	rm $(OBJ) $(EXE)

$(EXE): $(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(LFLAG)

.c.o:
	$(CC) -g -c -o $@ $< $(CFLAG)
