CC = gcc
LIBS = -Wall -lncurses
TEST_LIBS = -lcriterion -I src/

INCLUDEDIR = include
SOURCEDIR = src
TESTDIR = src/tests

DEPS = $(wildcard $(INCLUDEDIR)/*.h) 
SOURCES = $(wildcard $(SOURCEDIR)/*.c)
TESTS = $(wildcard $(TESTDIR)/*.c)
TEST_SOURCES := $(filter-out src/main.c, $(SOURCES))

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(LIBS)

all: $(SOURCES)
	mkdir -p build ; $(CC) -o build/clunk $(SOURCES) $(LIBS)

debug: $(SOURCES)
	mkdir -p debug ; $(CC) -o debug/clunk_debug $(SOURCES) -g $(LIBS)

tests: $(TESTS)
	mkdir -p test ; $(CC) -o test/clunk_tests $(TEST_SOURCES) $(TESTS) $(LIBS) $(TEST_LIBS)

.PHONY: clean

clean:
	rm -rf build/ debug/ test/
