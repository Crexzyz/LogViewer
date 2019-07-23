CC = gcc
CFLAGS = -Iinclude
CFLAGS += -Wall -Wextra -Wno-sign-compare
LDFLAGS = -lncurses

SOURCE_DIR = src
OBJS_DIR = build

VIEWER_NAME = log_viewer
VIEWER_SRC_DIR := $(SOURCE_DIR)/$(VIEWER_NAME)

TAB_MANAGER_NAME = tab_manager
TAB_MANAGER_SRC_DIR := $(SOURCE_DIR)/$(TAB_MANAGER_NAME)

DIRS := $(TAB_MANAGER_SRC_DIR) $(VIEWER_SRC_DIR) 
SRCS := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c))

all: $(SRCS) $(OBJS_DIR) 
# Compile each .c file in source folder
	$(CC) $(CFLAGS) -c $(SRCS)
# Move objects to correct folder
	mv *.o build
# Link/Compile each .o and generate an executable named log_viewer
	$(CC) $(CFLAGS) $(OBJS_DIR)/*.o $(LDFLAGS) -o log_viewer
	@echo "\e[38;5;82mLog viewer compiled.\e[m"

# Fix incompatibility on WSL terminals
fixterm:
	infocmp | sed 's/\(\s\+\)rep=[^,]*,\s*/\1/' | tic - -o ~/.terminfo/

# Delete all object files and the executable
.PHONY: clean
clean:
	rm -rf $(OBJS_DIR) log_viewer

# Creates folders for object files
$(OBJS_DIR):
	mkdir -p $@
