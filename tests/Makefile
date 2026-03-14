TEST_TARGETS := bounce
BIN_DIR := bin

CFLAGS  := -Wall -Wextra -pedantic -I../include -l teye
CFLAGS  += -std=c99 -D_POSIX_C_SOURCE=200809L
CFLAGS  += -O2 -fsanitize=address -g

all: $(TEST_TARGETS)

bounce: $(BIN_DIR)/bounce.o $(BIN_DIR)/timer.o
	$(CC) $(BIN_DIR)/bounce.o $(BIN_DIR)/timer.o -o $(BIN_DIR)/bounce $(CFLAGS)
	echo "Tests compiled succesfully"


$(BIN_DIR)/%.o: %.c
	@mkdir -p bin
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf bin