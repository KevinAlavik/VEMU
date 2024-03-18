CC ?= gcc
CFLAGS := -Wall -pedantic -Werror -Wshadow -Wstrict-aliasing -Wstrict-overflow -O3
CFLAGS += 
LDFLAGS := 

BIN_DIR := bin
OBJ_DIR := build
SRC_DIR := src
DESTDIR ?= /usr/local/bin

TARGET_NAME := visc
TARGET := $(BIN_DIR)/$(TARGET_NAME)

SRC := $(foreach x, $(SRC_DIR), $(wildcard $(addprefix $(x)/*,.c*)))
OBJ := $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(@D)
	@printf "  LD $(notdir $@)\n"
	@$(CC) $(LDFLAGS) $(OBJ) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	@printf "  CC $<\n"
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY: install
install: $(TARGET)
	@install -m 755 $(TARGET) $(DESTDIR)

.PHONY: format
format:
	@clang-format -i $(shell find src -name "*.c" -o -name "*.h")

.PHONY: clean
clean:
	@rm -rf $(BIN_DIR) $(BUILD_DIR) $(OBJ_DIR)