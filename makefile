CC = gcc
LIBS = -Wall -lncurses

INCLUDEDIR = include
SOURCEDIR = src

DEPS = $(wildcard $(INCLUDEDIR)/*.h) $(wildcard $(INCLUDEDIR)/*/*.h)
SOURCES = $(wildcard $(SOURCEDIR)/*.c) $(wildcard $(SOURCEDIR)/*/*.c)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(LIBS)

all: $(SOURCES)
	mkdir -p build ; $(CC) -o build/notes $(SOURCES) $(LIBS)

debug: $(SOURCES)
	mkdir -p debug ; $(CC) -o debug/notes_debug $(SOURCES) -g $(LIBS)

.PHONY: clean

clean:
	rm -rf build/ debug/
