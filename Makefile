CC = gcc
CFLAGS = -Iinclude
CFLAGS += -Wall -Wextra -Wno-sign-compare
LDFLAGS = -lncurses

SOURCE_DIR = src
BUILD_DIR = build

# If necessary, exclude some files or folders inside src/ folder
EXCLUDED_DIRS := 
EXCLUDED_DIRS := $(patsubst %, $(SOURCE_DIR)/%, $(EXCLUDED_DIRS))

# Get all directories located in src/ except the ones defined in EXCLUDED_DIRS
DIRS := $(filter-out $(EXCLUDED_DIRS), $(wildcard $(SOURCE_DIR)/*))
# Get all .c files that are in src/*/
SRCS := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c))
# Generate .o file names for each .c file
OBJS := $(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
# Generate build/ folders for each folder in src/
OBJ_DIRS := $(patsubst $(SOURCE_DIR)/%, $(BUILD_DIR)/%, $(DIRS))

# Link/Compile each .o and generate an executable named log_viewer
all: $(OBJ_DIRS) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o log_viewerr
	@echo "\e[38;5;82mLog viewer compiled.\e[m"

# Compile each .c file in source folder
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

# Fix incompatibility on WSL terminals
fixterm:
	infocmp | sed 's/\(\s\+\)rep=[^,]*,\s*/\1/' | tic - -o ~/.terminfo/

# Delete all object files and the executable
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) log_viewerr

# Creates folders for object files
$(OBJ_DIRS):
	mkdir -p $@
