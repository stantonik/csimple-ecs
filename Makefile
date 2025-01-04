######################################################################
# @author      : stanleyarn (stanleyarn@$HOSTNAME)
# @file        : Makefile
# @created     : Lundi déc 23, 2024 16:24:14 CET
######################################################################

######################################################################
#### Variables and settings
######################################################################

# Executable name
EXAMPLE_EXEC = example
LIB_NAME = csimple_ecs

# Build directory
BUILD_DIR_ROOT = build

# Sources
SRC_DIR = src
SRCS := $(sort $(shell find $(SRC_DIR) -name '*.c'))
EXAMPLE_SRC = example/main.c

# Library directory
LIBS_DIR = lib

# Includes
INCLUDE_DIR = include
INCLUDES = $(addprefix -I,$(SRC_DIR) $(INCLUDE_DIR))

# C preprocessor settings
CPPFLAGS = $(INCLUDES) -MMD -MP

# C compiler settings
CC = gcc
CFLAGS = -g -Wall -std=gnu99

# Linker flags
LDFLAGS =
LDLIBS =

######################################################################
#### Final setup
######################################################################

# Add .exe extension to executables on Windows
ifeq ($(OS),windows)
	EXAMPLE_EXEC := $(EXAMPLE_EXEC).exe
endif

BUILD_DIR := $(BUILD_DIR_ROOT)/$(OS)
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin

# Object files
OBJS := $(SRCS:%.c=$(OBJ_DIR)/%.o)

# Dependency files
DEPS := $(OBJS:.o=.d)

# Compilation database fragments
COMPDBS := $(OBJS:.o=.json)

# All files (sources and headers) (for formatting and linting)
FILES := $(shell find $(SRC_DIR) $(INCLUDE_DIR) -name '*.c' -o -name '*.h' -o -name '*.inl' 2>/dev/null)

######################################################################
#### Targets
######################################################################

# Disable default implicit rules
.SUFFIXES:

# Create static library
.PHONY: all
all: $(OBJS)
	@echo "Creating static library: $(LIBS_DIR)/$(LIB_NAME).a"
	@mkdir -p $(LIBS_DIR)
	ar rcs $(LIBS_DIR)/$(LIB_NAME).a $(OBJS)

.PHONY: example
example: $(BIN_DIR)/$(EXAMPLE_EXEC)
	$(BIN_DIR)/$(EXAMPLE_EXEC)

# Build executable
$(BIN_DIR)/$(EXAMPLE_EXEC):
	@echo "Building executable: $@"
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(EXAMPLE_SRC) $(LIBS_DIR)/$(LIB_NAME).a -o $@

# Compile C source files
$(OBJS): $(OBJ_DIR)/%.o: %.c
	@echo "Detected platform : $(OS)"
	@echo "Compiling: $<"
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Include automatically-generated dependency files
-include $(DEPS)

.PHONY: clean
clean:
	$(RM) -rf $(BUILD_DIR)

.PHONY: compdb
compdb: $(BUILD_DIR_ROOT)/compile_commands.json

# Generate JSON compilation database (compile_commands.json) by merging fragments
$(BUILD_DIR_ROOT)/compile_commands.json: $(COMPDBS)
	@echo "Generating: $@"
	@mkdir -p $(@D)
	@printf "[\n" > $@
	@for file in $(COMPDBS); do sed -e '$$s/$$/,/' "$${file}"; done | sed -e '$$s/,$$//' -e 's/^/    /' >> $@
	@printf "]\n" >> $@

# Generate JSON compilation database fragments from source files
$(COMPDBS): $(OBJ_DIR)/%.json: %.c
	@mkdir -p $(@D)
	@printf "\
	{\n\
	    \"directory\": \"$(CURDIR)\",\n\
	    \"command\": \"$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $(basename $@).o\",\n\
	    \"file\": \"$<\"\n\
	}\n" > $@

.PHONY: help
help:
	@echo "\
	Usage: make target... [options]...\n\
	\n\
	Targets:\n\
	  all             Build static library (default target)\n\
	  example         Build and run example\n\
	  clean           Clean build directory (all platforms)\n\
	  compdb          Generate JSON compilation database (compile_commands.json)\n\
	  help            Print this information\n"
