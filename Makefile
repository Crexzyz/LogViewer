CC = gcc
CFLAGS = -Iinclude
CFLAGS += -Wall -Wextra -Wno-sign-compare
LDFLAGS = -lncurses

SOURCE_DIR = src
OBJS_DIR = build

FOLDERS := log_viewer tab_manager
DIRS := $(patsubst %, $(SOURCE_DIR)/%, $(FOLDERS))
SRCS := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c))
OBJS := $(patsubst $(SOURCE_DIR)/%.c, $(OBJS_DIR)/%.o, $(SRCS))
OBJ_DIRS := $(patsubst $(SOURCE_DIR)/%, $(OBJS_DIR)/%, $(DIRS))

# Link/Compile each .o and generate an executable named log_viewer
all: $(OBJ_DIRS) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o log_viewerr
	@echo "\e[38;5;82mLog viewer compiled.\e[m"

# Compile each .c file in source folder
$(OBJS_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

# Fix incompatibility on WSL terminals
fixterm:
	infocmp | sed 's/\(\s\+\)rep=[^,]*,\s*/\1/' | tic - -o ~/.terminfo/

# Delete all object files and the executable
.PHONY: clean
clean:
	rm -rf $(OBJS_DIR) log_viewerr

# Creates folders for object files
$(OBJ_DIRS):
	mkdir -p $@
