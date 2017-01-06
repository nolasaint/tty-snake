# variables
INC_DIR := inc
OBJ_DIR := bin
SRC_DIR := src

DEP     := $(wildcard $(INC_DIR)/*.h)
SRC     := $(wildcard $(SRC_DIR)/*.c)
OBJ     := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CC      := gcc
CFLAGS  := -I$(INC_DIR) -g3
LDFLAGS := -lncurses -lpthread

# define default target
all: makedir tty-snake

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEP)
	$(CC) -c -o $@ $< $(CFLAGS) $(LDFLAGS)

tty-snake: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

info:
	@echo 'DEP = $(DEP)'
	@echo 'SRC = $(SRC)'
	@echo 'OBJ = $(OBJ)'

makedir:
	mkdir -p $(OBJ_DIR)

.PHONY: all clean info makedir

clean:
	rm ./tty-snake $(OBJ_DIR)/*.o
