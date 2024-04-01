CC ?= gcc
CFLAGS := -Wall -Wno-newline-eof -pedantic -Werror -Wshadow -Wstrict-aliasing -Wstrict-overflow -O3
LDFLAGS := 

BIN_DIR := bin
OBJ_DIR := build
SRC_DIR := src
DESTDIR ?= /usr/local/bin

TARGET_NAME := vemu
TARGET := $(BIN_DIR)/$(TARGET_NAME)

SRC := $(wildcard $(addsuffix /*.c*, $(SRC_DIR)) $(addsuffix /*.c*, $(SRC_DIR)/devices))
OBJ := $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(@D)
	@printf "  LD $(notdir $@)\n"
	@$(CC) $(LDFLAGS) $(OBJ) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/devices/%.c
	@mkdir -p $(@D)
	@printf "  CC $<\n"
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	@printf "  CC $<\n"
	@$(CC) $(CFLAGS) -c $< -o $@
	
.PHONY: debug
debug: CFLAGS += -DDEBUG_LOG -DDUMP_REGS
debug: all

.PHONY: dump
dump: CFLAGS += -DDUMP_REGS
dump: all

.PHONY: install
install: $(TARGET)
	@install -m 755 $(TARGET) $(DESTDIR)

.PHONY: format
format:
	@clang-format -i $(shell find src -name "*.c" -o -name "*.h")

.PHONY: clean
clean:
	@rm -rf $(BIN_DIR) $(BUILD_DIR) $(OBJ_DIR)
