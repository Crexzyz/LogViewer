SOURCE_DIR = src
OBJS_DIR = build

VIEWER_NAME = log_viewer

VIEWER_SRC_DIR := $(SOURCE_DIR)/$(VIEWER_NAME)
VIEWER_OBJ_DIR := $(OBJS_DIR)/$(VIEWER_NAME)
VIEWER_SRCS := $(wildcard $(VIEWER_SRC_DIR)/*.c )
VIEWER_OBJS := $(patsubst $(VIEWER_SRC_DIR)/%.c, $(VIEWER_OBJ_DIR)/%.o, $(VIEWER_SRCS))

all: $(VIEWER_NAME)

# Link/Compile each .o and generate an executable named logviewerr
$(VIEWER_NAME): $(VIEWER_OBJ_DIR) $(VIEWER_OBJS)
	gcc -Wall -Wextra -Wno-sign-compare $(VIEWER_OBJS) -lncurses -o $@r
	@echo "\e[38;5;82mLog viewer compiled.\e[m"

# Compile each .c file in source folder

$(VIEWER_OBJ_DIR)/%.o: $(VIEWER_SRC_DIR)/%.c
	gcc -Wall -Wextra -Wno-sign-compare -c $< -lncurses -o $@

# Fix incompatibility on WSL terminals
fixterm:
	infocmp | sed 's/\(\s\+\)rep=[^,]*,\s*/\1/' | tic - -o ~/.terminfo/

# Delete all object files and the executable
.PHONY: clean
clean:
	rm -rf $(OBJS_DIR) log_viewerr

# Creates folders for object files
$(VIEWER_OBJ_DIR):
	mkdir -p $@
