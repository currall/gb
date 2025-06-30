CC = gcc
CFLAGS := -Wall -Wextra -Iinclude

SRC_DIR := src
OBJ_DIR := build
BIN := main

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(OBJ_DIR) $(BIN)

.PHONY: all clean
