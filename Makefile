CC=gcc
CFLAGS=-O3 -funroll-loops -c -Wall
LDFLAGS=-O2 -lm
SOURCES=MyBot.c YourCode.c ants.c ant.c
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
