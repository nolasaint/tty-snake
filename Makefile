# variables
INC_DIR := inc
OBJ_DIR := bin
SRC_DIR := src

DEP     := $(wildcard $(INC_DIR)/*.h)
SRC     := $(wildcard $(SRC_DIR)/*.c)
OBJ     := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CC      := gcc
CFLAGS  := -I$(INC_DIR)
LDFLAGS := -lncurses -lpthread


#
# Special compilation rules
#

# list of non-file ("phony") targets
.PHONY: all clean debug info makedir

# define default target
all: makedir tty-snake



#
# Custom compilation rules
#

# define how all c files will compile
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEP)
	$(CC) -c -o $@ $< $(CFLAGS) $(LDFLAGS)

# primary compilation target
tty-snake: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

# remove compiled files
clean:
	rm ./tty-snake $(OBJ_DIR)/*.o

# debugging uses g3 no-optimization flag
debug:	CFLAGS += -g3
debug:	all

# display files used in compilation
info:
	@echo 'DEP = $(DEP)'
	@echo 'SRC = $(SRC)'
	@echo 'OBJ = $(OBJ)'

# create the OBJ_DIR directory
makedir:
	mkdir -p $(OBJ_DIR)

