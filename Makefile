# Ensure GNU Make
ifneq (,)
This makefile requires GNU Make
endif

CC      := gcc
CFLAGS  := -Wall -Wextra -pedantic -fPIC -O2 -Iinclude
CFLAGS  += -std=c99 -D_POSIX_C_SOURCE=200809L
LFLAGS  := -shared

PREFIX  ?= /usr/local
LIBDIR  := $(PREFIX)/lib
INCDIR  := $(PREFIX)/include/teye

LIBRARY := libteye.so
SRCS    := $(wildcard src/*.c)
LOBJS   := $(SRCS:src/%.c=bin/%.o)
HEADERS := $(wildcard include/teye/*.h)

.PHONY: all teye install clean

all: teye

# Build the shared library
teye: $(LOBJS)
	@mkdir -p lib
	$(CC) $(LFLAGS) -o lib/$(LIBRARY) $(LOBJS)

# Pattern rule for object files
bin/%.o: src/%.c
	@mkdir -p bin
	$(CC) -c $< -o $@ $(CFLAGS)


tests: install
	make -C tests

define install_rule
	install -Dm644 $(1) $(DESTDIR)$(INCDIR)/$(notdir $(1))

endef

install: teye
	@echo "Installing to $(PREFIX)..."
	# Install the library
	install -Dm755 lib/$(LIBRARY) $(DESTDIR)$(LIBDIR)/$(LIBRARY)
	
	# Install headers
	$(foreach header,$(HEADERS),$(call install_rule,$(header)))
	
	@echo "Updating linker cache..."
	-ldconfig $(LIBDIR)

clean:
	rm -rf bin lib tests/bin
