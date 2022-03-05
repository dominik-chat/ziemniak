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

PRJ = stage2

BUILD_DIR = build
SRC_DIR = src
INC_DIR = include

ASMSRC = $(wildcard $(SRC_DIR)/*.asm)
ASMOBJ = $(ASMSRC:$(SRC_DIR)/%.asm=$(BUILD_DIR)/%.o)

CSRC = $(wildcard $(SRC_DIR)/*.c)
COBJ = $(CSRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

ASMFLAGS = -f elf32
ASM = nasm

CFLAGS = \
	-std=gnu99 \
	-ffreestanding\
	-O2 \
	-Wall \
	-Wextra \
	-c \
	-mgeneral-regs-only \
	-mno-red-zone
CC = i686-elf-gcc

LDFLAGS = \
	-ffreestanding \
	-O2 \
	-nostdlib \
	-lgcc
LDFILE = linker.ld
OBJ = $(ASMOBJ) $(COBJ)
LD = i686-elf-gcc

COPY = i686-elf-objcopy

all: prepare $(ASMOBJ) $(COBJ) $(PRJ).elf $(PRJ).bin end


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo -e "\e[32mCompiling C object: $<\e[0m"
	@$(CC) $(CFLAGS) -I./$(INC_DIR) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm
	@echo -e "\e[31mAssembling: $<\e[0m"
	@$(ASM) $(ASMFLAGS) $< -o $@

%.elf: $(OBJ)
	@echo -e "\e[35mLinking: $@\e[0m"
	@$(LD) -T $(LDFILE) $(LDFLAGS) $(OBJ) -o $(BUILD_DIR)/$@

%.bin: %.elf
	@echo -e "\e[34mGenerating binary: $@\e[0m"
	@$(COPY) -O binary $(BUILD_DIR)/$< $(BUILD_DIR)/$@

prepare:
	@echo -e "\e[33m\e[1mBuilding $(PRJ) module\e[0m"
	@mkdir -p $(BUILD_DIR)

end:
	@echo -e "\e[33m\e[1mModule $(PRJ) built successfully\e[0m"

clean:
	@rm -rf $(BUILD_DIR)
	@rm -f *.bin
