######################################################################
# @author      : stanleyarn (stanleyarn@$HOSTNAME)
# @file        : Makefile
# @created     : Lundi déc 23, 2024 16:24:14 CET
######################################################################

######################################################################
#### Variables and settings
######################################################################

# Executable name
EXEC = test

# Build directory
BUILD_DIR_ROOT = build

# Sources
SRC_DIR = src
SRCS := $(sort $(shell find $(SRC_DIR) -name '*.c'))
SRCS += example/main.c

# Library directory
LIBS_DIR = libs

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
	EXEC := $(EXEC).exe
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

.PHONY: all
all: $(BIN_DIR)/$(EXEC)

# Build executable
$(BIN_DIR)/$(EXEC): $(OBJS)
	@echo "Detected platform : $(OS)"
	@echo "Building executable: $@"
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Compile C source files
$(OBJS): $(OBJ_DIR)/%.o: %.c
	@echo "Compiling: $<"
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Include automatically-generated dependency files
-include $(DEPS)

.PHONY: run
run: $(BIN_DIR)/$(EXEC)
	@cd $(BIN_DIR) && ./$(EXEC)

.PHONY: clean
clean:
	$(RM) -rf $(BUILD_DIR_ROOT)

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
