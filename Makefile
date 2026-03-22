ifneq (,)
this makefile requires GNU Make
endif

# Build configuration
DEBUG ?= 0
STATIC ?= 0
BUILD_TYPE ?= both  # options: shared, static, both

ifeq ($(DEBUG),1)
DEBUG_FLAGS := -g -fsanitize=address -O0
else
DEBUG_FLAGS := -O2
endif

CC      := gcc
AR      := ar
ARFLAGS := rcs
CFLAGS  := -Wall -Wextra -pedantic -fPIC -Iinclude
CFLAGS  += -std=c99 -D_POSIX_C_SOURCE=200809L $(DEBUG_FLAGS)
LFLAGS  := -shared

PREFIX  ?= /usr/local
LIBDIR  := $(PREFIX)/lib
INCDIR  := $(PREFIX)/include/teye

LIBRARY_SHARED := libteye.so
LIBRARY_STATIC := libteye.a
SRCS    := $(wildcard src/*.c)
LOBJS   := $(SRCS:src/%.c=bin/%.o)
HEADERS := $(wildcard include/teye/*.h)

.PHONY: all shared static tests install uninstall clean

all: $(BUILD_TYPE)

# Build shared library
shared: $(LOBJS)
	@mkdir -p lib
	$(CC) $(LFLAGS) -o lib/$(LIBRARY_SHARED) $(LOBJS)

# Build static library
static: $(LOBJS)
	@mkdir -p lib
	$(AR) $(ARFLAGS) lib/$(LIBRARY_STATIC) $(LOBJS)

# Build both libraries
both: shared static

# Pattern rule for object files (position-independent code for both)
bin/%.o: src/%.c
	@mkdir -p bin
	$(CC) -c $< -o $@ $(CFLAGS)

# Run tests
tests: shared static
	$(MAKE) -C tests

# Installation
install: $(BUILD_TYPE)
	@echo "Installing to $(PREFIX)..."
	@mkdir -p $(DESTDIR)$(LIBDIR) $(DESTDIR)$(INCDIR)
	
	@if [ -f lib/$(LIBRARY_SHARED) ] && [ "$(BUILD_TYPE)" != "static" ]; then \
		echo "Installing $(LIBRARY_SHARED)..."; \
		install -m755 lib/$(LIBRARY_SHARED) $(DESTDIR)$(LIBDIR)/$(LIBRARY_SHARED); \
	fi
	
	@if [ -f lib/$(LIBRARY_STATIC) ] && [ "$(BUILD_TYPE)" != "shared" ]; then \
		echo "Installing $(LIBRARY_STATIC)..."; \
		install -m644 lib/$(LIBRARY_STATIC) $(DESTDIR)$(LIBDIR)/$(LIBRARY_STATIC); \
	fi
	
	@for header in $(HEADERS); do \
		install -Dm644 $$header $(DESTDIR)$(INCDIR)/$$(basename $$header); \
	done
	
	@if [ -f lib/$(LIBRARY_SHARED) ] && [ "$(BUILD_TYPE)" != "static" ]; then \
		echo "Updating linker cache..."; \
		-ldconfig $(DESTDIR)$(LIBDIR) 2>/dev/null || true; \
	fi

# Uninstallation
uninstall:
	@echo "Removing from $(PREFIX)..."
	rm -f $(DESTDIR)$(LIBDIR)/$(LIBRARY_SHARED)
	rm -f $(DESTDIR)$(LIBDIR)/$(LIBRARY_STATIC)
	@for header in $(HEADERS); do \
		rm -f $(DESTDIR)$(INCDIR)/$$(basename $$header); \
	done
	-ldconfig 2>/dev/null || true

# Clean build artifacts
clean:
	rm -rf bin lib
	$(MAKE) -C tests clean 2>/dev/null || true
