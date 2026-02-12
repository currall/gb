CC = gcc

CFLAGS := -Iinclude -I/mingw64/include/SDL2 -Iexternal/tinyfiledialogs
CFLAGS_DEBUG := -Wall -Wextra -Iinclude -I/mingw64/include/SDL2 -Iexternal/tinyfiledialogs

LDLIBS := -lSDL2main -lSDL2 -lole32 -lcomdlg32

SRC_DIR := src
OBJ_DIR := build
BIN := main

EXT_DIR := external/tinyfiledialogs
EXT_SRCS := $(EXT_DIR)/tinyfiledialogs.c
SRCS := $(wildcard $(SRC_DIR)/*.c) $(EXT_SRCS)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/tinyfiledialogs.o: $(EXT_DIR)/tinyfiledialogs.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN)

.PHONY: all clean
