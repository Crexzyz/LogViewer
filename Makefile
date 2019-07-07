SOURCE_DIR = src
OBJS_DIR = build

VIEWER_NAME = log_viewer

VIEWER_SRC_DIR := $(SOURCE_DIR)/$(VIEWER_NAME)
VIEWER_OBJ_DIR := $(OBJS_DIR)/$(VIEWER_NAME)
VIEWER_SRCS := $(wildcard $(VIEWER_SRC_DIR)/*.c )
VIEWER_OBJS := $(patsubst $(VIEWER_SRC_DIR)/%.c, $(VIEWER_OBJ_DIR)/%.o, $(VIEWER_SRCS))

all: $(VIEWER_NAME)

$(VIEWER_NAME): $(VIEWER_OBJ_DIR) $(VIEWER_OBJS)
	gcc -Wall -Wextra -Wno-sign-compare $(VIEWER_OBJS) -lncurses -o $@r
	@echo "\e[38;5;82mLog viewer compiled.\e[m"

$(VIEWER_OBJ_DIR)/%.o: $(VIEWER_SRC_DIR)/%.c
	gcc -Wall -Wextra -Wno-sign-compare -c $< -lncurses -o $@

.PHONY: clean
clean:
	rm -rf $(OBJS_DIR) log_viewerr

# Creates temporal folders for objects
$(VIEWER_OBJ_DIR):
	mkdir -p $@
