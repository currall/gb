CC = gcc

CFLAGS := -Iinclude -I/mingw64/include/SDL2
CFLAGS_DEBUG := -Wall -Wextra -Iinclude -I/mingw64/include/SDL2

LDLIBS := -lSDL2main -lSDL2

SRC_DIR := src
OBJ_DIR := build
BIN := main

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN)

.PHONY: all clean
