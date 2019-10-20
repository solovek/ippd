CC  = gcc
SRC = $(shell find . -name "*.c") ../util.c
OBJ = $(addsuffix .o,$(basename $(SRC)))

DEP = OpenCL

CFLAG = `pkg-config $(DEP) --cflags`
LFLAG = `pkg-config $(DEP) --libs`

EXE = main

.SUFFIX: .c .o
.PHONY: all test clean mult integral

all: $(EXE) test

test: mult integral

mult: $(EXE)
	./$(EXE) vmult

#defaults
TARGET = linear
SAMPLE = 0.5
START  = 0
END    = 10

integral: $(EXE)
	./$(EXE) integral $(TARGET) $(SAMPLE) $(START) $(END)

clean:
	rm $(OBJ) $(EXE)

$(EXE): $(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(LFLAG)

.c.o:
	$(CC) -g -c -o $@ $< $(CFLAG)
