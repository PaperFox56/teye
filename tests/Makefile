# tests/Makefile
# Test suite for teye library

TEST_TARGETS := bounce mandelbrot
BIN_DIR := bin

# Build configuration
DEBUG ?= 0
STATIC ?= 0

CC       := gcc
CFLAGS   := -Wall -Wextra -pedantic -I../include
CFLAGS   += -std=c99 -D_POSIX_C_SOURCE=200809L

# Debug flags
ifeq ($(DEBUG),1)
CFLAGS   += -g -fsanitize=address -O0
else
CFLAGS   += -O2
endif

# Get absolute path to library directory
LIB_DIR  := $(abspath ../lib)
LIB_NAME := teye

# Library linking
LDFLAGS  := -L"$(LIB_DIR)"

ifeq ($(STATIC),1)
    # Link against static library
    LIBS := "$(LIB_DIR)/lib$(LIB_NAME).a"
    # Remove LDFLAGS for static linking
    LDFLAGS :=
else
    # Link against shared library
    LIBS := -l$(LIB_NAME)
    # Add rpath for development (quoted to handle spaces)
    LDFLAGS += -Wl,-rpath,"$(LIB_DIR)"
endif

# Additional libraries if needed
# LDFLAGS += -lm

.PHONY: all clean test run

all: $(TEST_TARGETS)
	@echo "Tests compiled successfully"

bounce: bounce.c timer.c
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $(BIN_DIR)/$@ $(CFLAGS) $(LDFLAGS) $(LIBS)

mandelbrot: mandelbrot.c timer.c
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $(BIN_DIR)/$@ $(CFLAGS) $(LDFLAGS) $(LIBS)

test: all
	@echo "Running tests..."
	./bounce

clean:
	rm -rf $(BIN_DIR) $(TEST_TARGETS)
