ifneq (,)
This makefile require GNU Make
endif

CC=gcc
CFLAGS= -Wall -Wextra -pedantic -fPIC

SOURCES=src
HEADERS=

LIBRARY=teye.a
LOBJS=bin/teye.o bin/timer.o
HEADERS=src/teye.h src/timer.h

$(LIBRARY): $(LOBJS)
	$(CC) -shared $(LOBJS) -o bin/$(LIBRARY) $(CFLAGS)

clock: $(LIBRARY) bin/clock.o
	$(CC) bin/clock.o -o bin/teye-clock $(CFLAGS) -lm bin/teye.a

bin/%.o: ./src/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

%.o: ./bin/%.o

includes: $(HEADERS)
	mkdir -p includes
	cp $(HEADERS) includes/

clean:
	rm bin/*
