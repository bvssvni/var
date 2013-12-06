EXE = testc
CFLAGS = -Wall -O3
COMPILER = gcc

all: $(EXE)

time: $(EXE)
	time ./$(EXE)

$(EXE): testc.o var.o
	$(COMPILER) *.o -o $(EXE)

clean:
	rm *.o && rm $(EXE)

var.o: var.c
	$(COMPILER) -c var.c $(CFLAGS)

testc.o: testc.c
	$(COMPILER) -c testc.c $(CFLAGS)
