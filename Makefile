SHELL := /bin/bash

CC := gcc
CFLAGS := -g -std=gnu11 -Wall -Wextra -Wpedantic
CLIBS := -lm -lSDL2 -lSDL2_image

# Final binary
BIN := main
# Put all auto generated stuff to this build dir.
BUILD_DIR := ./build

# List of all .cpp source files.
CS :=  $(shell find src -name "*.c")

# All .o files go to build dir.
OBJ := $(CS:%.c=$(BUILD_DIR)/%.o)
# Gcc/Clang will create these .d files containing dependencies.
DEP := $(OBJ:%.o=%.d)

# Default target
.PHONY : all
all : $(BUILD_DIR)/$(BIN)

# Actual target of the binary - depends on all .o files.
$(BUILD_DIR)/$(BIN) : $(OBJ)
	# Create build directories - same structure as sources.
	mkdir -p $(@D)
	# Just link all the object files.
	$(CC) $(CFLAGS) $^ -o $@ $(CLIBS)

# Include all .d files
-include $(DEP)

# Build target for every single object file.
# The potential dependency on header files is covered
# by calling `-include $(DEP)`.
$(BUILD_DIR)/%.o : %.c
	mkdir -p $(@D)
	# The -MMD flags additionaly creates a .d file with
	# the same name as the .o file.
	$(CC) $(CFLAGS) -MMD -c $< -o $@

.PHONY : clean
clean :
	# This should remove all generated files.
	-rm $(BUILD_DIR)/$(BIN) $(OBJ) $(DEP)


.PHONY : run
run : all
	./$(BUILD_DIR)/$(BIN)
