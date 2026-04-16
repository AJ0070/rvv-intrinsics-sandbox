PROJECT      := rvv-intrinsics-sandbox
BUILD_DIR    := build
SRC_DIR      := src
INC_DIR      := include

# Toolchain selection:
# - If TOOLCHAIN_PREFIX is provided, use it directly.
# - Else prefer riscv64-linux-gnu- (easy with qemu-riscv64 user mode)
# - Else fallback to riscv64-unknown-elf-
TOOLCHAIN_PREFIX ?=
ifeq ($(TOOLCHAIN_PREFIX),)
  ifneq ($(shell command -v riscv64-linux-gnu-gcc 2>/dev/null),)
    TOOLCHAIN_PREFIX := riscv64-linux-gnu-
  else ifneq ($(shell command -v riscv64-unknown-elf-gcc 2>/dev/null),)
    TOOLCHAIN_PREFIX := riscv64-unknown-elf-
  else
    TOOLCHAIN_PREFIX := riscv64-linux-gnu-
  endif
endif

CC           := $(TOOLCHAIN_PREFIX)gcc
OBJDUMP      := $(TOOLCHAIN_PREFIX)objdump

# RVV architecture flags
# Override with: make ARCH=rv64gcv_zba_zbb_zbc_zbs
ARCH         ?= rv64gcv
ABI          ?= lp64d

COMMON_CFLAGS := -std=c11 -O2 -Wall -Wextra -Werror -pedantic \
                 -I$(INC_DIR) -march=$(ARCH) -mabi=$(ABI)

# For linux-gnu toolchain we prefer static binaries for easier qemu-user execution.
ifeq ($(findstring linux-gnu,$(TOOLCHAIN_PREFIX)),linux-gnu)
  COMMON_LDFLAGS := -static
else
  COMMON_LDFLAGS :=
endif

TARGET       := $(BUILD_DIR)/rvv_sandbox.elf
SOURCES      := $(wildcard $(SRC_DIR)/*.c)
OBJECTS      := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

.PHONY: all clean info disasm

all: info $(TARGET)

info:
	@echo "[info] CC=$(CC)"
	@echo "[info] ARCH=$(ARCH) ABI=$(ABI)"
	@echo "[info] SOURCES=$(SOURCES)"

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(COMMON_CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(COMMON_LDFLAGS) -o $@

disasm: $(TARGET)
	$(OBJDUMP) -d $(TARGET) > $(BUILD_DIR)/$(PROJECT).S
	@echo "[ok] Disassembly at $(BUILD_DIR)/$(PROJECT).S"

clean:
	rm -rf $(BUILD_DIR)
