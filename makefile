EXECUTABLE = testc
SOURCES = var.c testc.c

all: $(EXECUTABLE)

time: $(EXECUTABLE)
	time ./$(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	gcc $(SOURCES) -o $@ -Wall -O3

clean:
	rm $(EXECUTABLE)

