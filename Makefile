ifneq (,)
This makefile require GNU Make
endif

CC=gcc
LFLAGS= -Wall -Wextra -pedantic -fPIC
CFLAGS= -Wall -Wextra -pedantic

HEADERS=

LIBRARY=libteye.so
LOBJS=lib/teye.o lib/timer.o lib/buffer/char_buffer.o
HEADERS=src/teye.h src/timer.h

TARGETS=clock
BINARIES=$(patsubst %,bin/teye-%,$(TARGETS))

all: $(TARGETS) teye

teye: $(LOBJS) $(HEADERS)
	$(CC) -shared $(LOBJS) -o lib/$(LIBRARY) $(LFLAGS)

clock: bin/clock.o
	$(CC) bin/clock.o -o bin/teye-clock $(CFLAGS) -l teye

lib/%.o: ./src/%.c	
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(LFLAGS)
bin/%.o: ./src/%.c	
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

#%.o: ./bin/%.o


# Installation

includes: $(HEADERS)
	mkdir -p includes
	cp $(HEADERS) includes/

install-lib:
	mkdir -p /usr/local/include/teye
	cp $(HEADERS) /usr/local/include/teye/
	cp lib/$(LIBRARY) /usr/lib/

install-binaries:
	cp -t /usr/local/bin $(BINARIES)

install: install-lib install-binaries

clean:
	rm -r bin
	rm -r lib
