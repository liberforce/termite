CC=gcc

# For debug
CFLAGS=-O0 -funroll-loops -c -Wall -ggdb3
LDFLAGS=-O0 -lm

# For release
#CFLAGS=-O3 -funroll-loops -c -Wall -DNDEBUG
#LDFLAGS=-O2 -lm

SOURCES=MyBot.c termite.c ant.c map.c utils.c state.c tile.c pathfinder.c hill.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=termite
ARCHIVE=$(EXECUTABLE).zip

all: $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f ${EXECUTABLE} ${OBJECTS} *.d

dist:
	rm -f $(ARCHIVE)
	file-roller --add-to=$(ARCHIVE) *.c *.h Makefile VERSION README
.PHONY: all clean
