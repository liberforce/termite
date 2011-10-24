CC=gcc
CFLAGS=-O3 -funroll-loops -c
LDFLAGS=-O2 -lm
SOURCES=termite.c YourCode.c ants.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=termite

all: $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f ${EXECUTABLE} ${OBJECTS} *.d

.PHONY: all clean
