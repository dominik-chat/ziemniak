# Copyright (C) 2022 Dominik Chat
#
# This file is part of Ziemniak.
#
# Ziemniak is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation, either version 3
# of the License, or (at your option) any later version.
#
# Ziemniak is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Ziemniak. If not, see <https://www.gnu.org/licenses/>.

PRJ = kernel

BUILD_DIR = build
SRC_DIR = src
INC_DIR = include

ASMSRC = $(wildcard $(SRC_DIR)/*/*.asm)
ASMOBJ = $(ASMSRC:$(SRC_DIR)/%.asm=$(BUILD_DIR)/%.o)

CSRC = $(wildcard $(SRC_DIR)/*/*.c)
COBJ = $(CSRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

ASMFLAGS = -f elf64
ASM = nasm

CFLAGS = \
	-std=gnu99 \
	-ffreestanding \
	-mcmodel=kernel \
	-O2 \
	-Wall \
	-Wextra \
	-c \
	-mno-mmx \
	-mno-sse \
	-mno-sse2 \
	-mno-red-zone \
	-mgeneral-regs-only
CC = x86_64-elf-gcc

LDFLAGS = \
	-O2 \
	-lgcc \
	--gc-sections
LDFILE = linker.ld
OBJ = $(ASMOBJ) $(COBJ)
LGCC =$(dir $(shell $(CC) $(CFLAGS) -print-libgcc-file-name))
LD = x86_64-elf-ld

COPY = x86_64-elf-objcopy
DUMP = x86_64-elf-objdump

all: prepare $(ASMOBJ) $(COBJ) $(PRJ).elf $(PRJ).bin end

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo -e "\e[32mCompiling C object: $<\e[0m"
	@$(CC) $(CFLAGS) -I./$(INC_DIR) -include config.h $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm
	@echo -e "\e[31mAssembling: $<\e[0m"
	@$(ASM) $(ASMFLAGS) $< -o $@

%.elf: $(OBJ)
	@echo -e "\e[35mLinking: $@\e[0m"
	@$(LD) -T $(LDFILE) $(LDFLAGS) $(OBJ) -L$(LGCC) -o $(BUILD_DIR)/$@

%.bin: %.elf
	@echo -e "\e[34mGenerating binary: $@\e[0m"
	@$(COPY) -O binary $(BUILD_DIR)/$< $(BUILD_DIR)/$@

prepare:
	@echo $(LGCC)
	@mkdir -p $(dir $(OBJ))
	@echo -e "\e[33m\e[1mBuilding $(PRJ) module\e[0m"

end:
	@$(DUMP) $(BUILD_DIR)/$(PRJ).elf -D > $(BUILD_DIR)/$(PRJ).asm
	@echo -e "\e[33m\e[1mModule $(PRJ) built successfully\e[0m"

clean:
	@rm -rf $(BUILD_DIR)
	@rm -f *.bin
