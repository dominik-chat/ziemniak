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

PRJ = Ziemniak OS

BUILD_DIR = build
ISO_DIR = iso

all: clean prepare submodules copy cd end

prepare:
	@echo -e "\e[33m\e[1mBuilding $(PRJ)\e[0m"
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(ISO_DIR)

submodules:
	@cd bootloader && $(MAKE) all
	@cd kernel && $(MAKE) all

copy:
	@cp bootloader/$(BUILD_DIR)/* $(BUILD_DIR)/
	@cp kernel/$(BUILD_DIR)/kernel.bin $(BUILD_DIR)/potato.bin

cd:
	@echo -e "\e[34mGenerating iso\e[0m"
	@mkdir -p $(ISO_DIR)/boot
	@cp $(BUILD_DIR)/stage1.bin $(ISO_DIR)/boot/stage1.bin
	@cp $(BUILD_DIR)/potato.bin $(ISO_DIR)/potato.bin
	@mkisofs -V "Ziemniak OS" -R -J --quiet -b boot/stage1.bin -G $(BUILD_DIR)/stage2.bin -no-emul-boot -boot-load-size 4 -o ./bootable.iso ./$(ISO_DIR)

end:
	@echo -e "\e[33m\e[1m$(PRJ) built successfully\e[0m"

clean:
	@cd bootloader && $(MAKE) clean
	@cd kernel && $(MAKE) clean
	@rm -rf $(BUILD_DIR)
	@rm -rf $(ISO_DIR)
	@rm -rf *.iso
